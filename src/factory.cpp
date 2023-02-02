#include "factory.hpp"

#include <stdexcept>
#include <sstream>

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors)
{
    if (node_colors[sender] == NodeColor::VERIFIED){ return true;}

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.begin() == sender->receiver_preferences_.end())
    {
        throw std::logic_error("No receivers");
    }

    bool has_receiver = false;
    for (auto receiver : sender->receiver_preferences_.get_preferences())
    {
        if (receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE)
        {
            has_receiver = true;
        }
        else if (receiver.first->get_receiver_type() == ReceiverType::WORKER)
        {
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (sendrecv_ptr == sender){continue;}

            has_receiver = true;

            if (auto iter = node_colors.find(sendrecv_ptr); iter != node_colors.end() &&  iter->second == NodeColor::UNVISITED)
            {
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if (has_receiver)
        return true;
    throw std::logic_error("No receviers");
}

bool Factory::is_consistent() const
{
   std::map<const PackageSender*, NodeColor> colour;

   for (auto &ramp : rampCollection_)
   {
       colour[&ramp] = NodeColor::UNVISITED;
   }

    for (auto &worker : workerCollection_)
    {
        colour[&worker] = NodeColor::UNVISITED;
    }

    try
    {
        for (const auto &ramp : rampCollection_)
        {
            has_reachable_storehouse(&ramp, colour);
        }
    }
    catch (std::logic_error &err)
    {
        return false;
    }
    return true;
}

void Factory::do_deliveries(Time time)
{
    for (auto &ramp : rampCollection_)
    {
        ramp.deliver_goods(time);
    }
}

void Factory::do_package_passing()
{
    for (auto &ramp : rampCollection_)
    {
        ramp.send_package();
    }
}

void Factory::do_work(Time time)
{
    for (auto &worker : workerCollection_)
    {
        worker.do_work(time);
    }
}

enum class ElementType
{
    RAMP, WORKER, STOREHOUSE, LINK
};


ElementType str2ElementType(std::string_view str)
{
    ElementType elementType;
    if (str == "LOADING_RAMP" || str == "ramp")
    {
        elementType = ElementType::RAMP;
    }
    else if (str == "WORKER" || str == "worker")
    {
        elementType = ElementType::WORKER;
    }
    else if (str == "STOREHOUSE" || str == "store")
    {
        elementType = ElementType::STOREHOUSE;
    }
    else if (str == "LINK")
    {
        elementType = ElementType::LINK;
    }
    else
    {
        throw std::runtime_error("Unknown tag!");
    }
    return elementType;
}

struct ParsedLineData
{
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};


ParsedLineData parse_line(std::string& line)
{
    constexpr char separator = ' ';
    constexpr char separator_of_param = '=';

    ParsedLineData lineData;
    std::istringstream token_stream(line);
    std::string token;


    std::getline(token_stream, token, separator);
    lineData.element_type = str2ElementType(token);

    while (std::getline(token_stream, token, separator))
    {
        std::size_t idx = token.find(separator_of_param);
        lineData.parameters[token.substr(0, idx)] = token.substr(idx + 1);
    }

    return lineData;
}

Factory load_factory_structure(std::istream& is)
{
    Factory factory;

    std::string line;
    while (std::getline(is, line))
    {
        if (line.empty() || line[0] == ';')
        {
            continue;
        }

        ParsedLineData lineData = parse_line(line);

        if (lineData.element_type == ElementType::RAMP)
        {
            ElementID id = std::stoull(lineData.parameters.at("id"));
            TimeOffset t = std::stoi(lineData.parameters.at("delivery-interval"));
            factory.add_ramp(Ramp(id, t));
        }

        else if(lineData.element_type == ElementType::WORKER)
        {
            ElementID id = std::stoull(lineData.parameters.at("id"));
            TimeOffset t = std::stoi(lineData.parameters.at("processing-time"));
            PackageQueueType queueType = lineData.parameters.at("queue-type") == "LIFO" ? PackageQueueType::LIFO : PackageQueueType::FIFO;

            std::unique_ptr<IPackageQueue> uniquePtr = std::make_unique<PackageQueue>(PackageQueue(queueType));

            factory.add_worker(Worker(id, t, std::move(uniquePtr)));
        }

        else if(lineData.element_type == ElementType::STOREHOUSE)
        {
            ElementID id = std::stoull(lineData.parameters.at("id"));
            factory.add_storehouse(Storehouse(id));
        }

        else if(lineData.element_type == ElementType::LINK)
        {
            std::string_view src = lineData.parameters.at("src");
            std::string_view dest = lineData.parameters.at("dest");

            std::size_t src_idx = src.find('-');
            std::size_t dest_idx = dest.find('-');

            ElementType src_node_type = str2ElementType(src.substr(0, src_idx));
            ElementType dest_node_type = str2ElementType(dest.substr(0, dest_idx));

            ElementID src_id = std::stoull(std::string(src.substr(src_idx+1)));
            ElementID dest_id = std::stoull(std::string(dest.substr(dest_idx+1)));

            IPackageReceiver* pReceiver = nullptr;
            if (dest_node_type == ElementType::WORKER)
            {
                pReceiver = &(*factory.find_worker_by_id(dest_id));
            }
            else if (dest_node_type == ElementType::STOREHOUSE)
            {
                pReceiver = &(*factory.find_storehouse_by_id(dest_id));
            }

            if (src_node_type == ElementType::RAMP)
            {
                factory.find_ramp_by_id(src_id)->receiver_preferences_.add_receiver(pReceiver);
            }
            else if (src_node_type == ElementType::WORKER)
            {
                factory.find_worker_by_id(src_id)->receiver_preferences_.add_receiver(pReceiver);
            }

        }

    }
    return factory;
}

std::vector<std::string> destinations_to_vector(const PackageSender *packageSender)
{
    std::string worker = "worker";
    std::string store = "store";

    std::vector<std::string> destinations;
    for (auto [key, val] : packageSender->receiver_preferences_.get_preferences())
    {
        std::string str = key->get_receiver_type() == ReceiverType::WORKER ? worker : store;
        str += "-";
        str += std::to_string(key->get_id());
        destinations.push_back(str);
    }
    return destinations;
}

void save_factory_structure(Factory& factory, std::ostream& os)
{
    os << "; == LOADING RAMPS ==\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](const Ramp &ramp)
    {
        os << "LOADING_RAMP id=" << ramp.get_id() << " delivery-interval=" << ramp.get_delivery_interval() << "\n";
    });

    os << "; == WORKERS ==\n\n";
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](const Worker &worker)
    {
        os << "WORKER id=" << worker.get_id() << " processing-time=" << worker.get_processing_duration() \
            << " queue-type=" << (worker.get_queue()->get_queue_type() == PackageQueueType::FIFO ? "FIFO" : "LIFO") << "\n";
    });

    os << "; == STOREHOUSES ==\n\n";
    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&os](const Storehouse &storehouse)
    {
        os << "STOREHOUSE id=" << storehouse.get_id() << "\n";
    });

    os << "; == LINKS ==\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](const Ramp &ramp)
    {
       std::vector<std::string> des = destinations_to_vector(static_cast<const PackageSender*>(&ramp));
       for (const auto& s : des)
       {
           os << "LINK src=ramp-" << ramp.get_id() << " dest=" << s << "\n";
       }
       os << "\n";
    });

    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](const Worker &worker)
    {
        std::vector<std::string> des = destinations_to_vector(static_cast<const PackageSender*>(&worker));
        for (const auto& s : des)
        {
            os << "LINK src=worker-" << worker.get_id() << " dest=" << s << "\n";
        }
        os << "\n";
    });

    os.flush();
}

