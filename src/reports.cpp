#include "reports.hpp"
#include <queue>

void generate_structure_report_ramp(const Ramp& ramp, std::ostream& os)
{
    os << "LOADING RAMP #" << ramp.get_id() << "\n";
    os << "  Delivery interval: " << ramp.get_delivery_interval() << "\n";
    os << "  Receivers:\n";
    for (auto [key, val] : ramp.receiver_preferences_)
    {
        switch (key->get_receiver_type())
        {
            case ReceiverType::WORKER:
                os << "    worker #" << key->get_id() << "\n";
                break;
            case ReceiverType::STOREHOUSE:
                os << "    storehouse #" << key->get_id() << "\n";
                break;
        }
    }
    os << std::endl;
}

void generate_structure_report_worker(const Worker& worker, std::ostream& os)
{
    os << "WORKER #" << worker.get_id() << "\n";
    os << "  Processing time: " << worker.get_processing_duration() << "\n";
    os << "  Queue type: " << (worker.get_queue()->get_queue_type() == PackageQueueType::FIFO ? "FIFO" : "LIFO") << "\n";
    os << "  Receivers:\n";

    std::priority_queue<ElementID, std::vector<ElementID>, std::greater<>> id_workers;
    std::priority_queue<ElementID, std::vector<ElementID>, std::greater<>> id_storehouses;

    for (auto [key, val] : worker.receiver_preferences_)
    {
        switch (key->get_receiver_type())
        {
            case ReceiverType::WORKER:
                id_workers.push(key->get_id());
                break;
            case ReceiverType::STOREHOUSE:
                id_storehouses.push(key->get_id());
                break;
        }
    }

    while (!id_workers.empty())
    {
        os << "    workers #" << std::to_string(id_workers.top()) << "\n";
        id_workers.pop();
    }

    while (!id_storehouses.empty())
    {
        os << "    storehouse #" << std::to_string(id_storehouses.top()) << "\n";
        id_storehouses.pop();
    }

    os << std::endl;
}

void generate_structure_report(const Factory& f, std::ostream& os)
{
    os << "\n== LOADING RAMPS ==\n\n";
    std::for_each(f.ramp_cbegin(), f.ramp_cend(), [&os](const Ramp& ramp){ generate_structure_report_ramp(ramp, os);});

    os << "\n== WORKERS ==\n\n";
    std::for_each(f.worker_cbegin(), f.worker_cend(), [&os](const Worker& worker) {generate_structure_report_worker(worker, os);} );

    os << "\n== STOREHOUSES ==\n\n";
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), [&os](const Storehouse& storehouse)
    {
       os << "STOREHOUSE #" << storehouse.get_id() << "\n\n";
    });
    os.flush();
}

void generate_simulation_turn_report_worker(const Worker& worker, std::ostream& os)
{
    os << "WORKER #" << worker.get_id() << "\n";
    os << "  PBuffer: ";

    if (worker.get_processing_buffer().has_value())
    {
        os << "#" << std::to_string(worker.get_processing_buffer()->get_id());
        os << " (pt = " << worker.get_package_processing_start_time() << ")\n";
    }
    else
    {
        os << "(empty)\n";
    }


    os << "  Queue: ";
    if (worker.get_queue()->begin() == worker.get_queue()->end())
    {
        os << "(empty)";
    }
    for (auto iter = worker.get_queue()->begin(); iter != worker.get_queue()->end(); ++iter)
    {
        os << "#" << iter->get_id();
        if (std::next(iter) != worker.get_queue()->end())
        {
            os << ", ";
        }
    }
    os << "\n";

    os << "  SBuffer: " << (worker.get_sending_buffer() ? "#" + std::to_string(worker.get_sending_buffer().value().get_id()) : "(empty)");
    os << "\n";
    os << std::endl;
}

void generate_simulation_turn_report_storehouse(const Storehouse& storehouse, std::ostream& os)
{
    os << "STOREHOUSE #" << storehouse.get_id() << "\n";
    os << "  Stock: ";
    if (storehouse.cbegin() == storehouse.cend())
    {
        os << "(empty)";
    }
    for (auto iter = storehouse.cbegin(); iter != storehouse.cend(); ++iter)
    {
        os << "#" << iter->get_id();
        if (std::next(iter) != storehouse.cend())
        {
            os << ", ";
        }
    }
    os << std::endl;
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t)
{
    os << "=== [ Turn: " << t << " ] ===\n\n";

    os << "== WORKERS ==\n\n";
    std::for_each(f.worker_cbegin(), f.worker_cend(), [&os](const Worker& worker){ generate_simulation_turn_report_worker(worker, os);});

    os << "\n== STOREHOUSES ==\n\n";
    std::for_each(f.storehouse_cbegin(), f.storehouse_cend(), [&os](const Storehouse& storehouse){ generate_simulation_turn_report_storehouse(storehouse, os);});
    os << std::endl;
}