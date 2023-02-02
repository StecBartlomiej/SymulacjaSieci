#ifndef SYMULACJASIECI_FACTORY_HPP
#define SYMULACJASIECI_FACTORY_HPP

#include "types.hpp"
#include "nodes.hpp"

#include <list>
#include <iostream>
#include <type_traits>


///
/// \tparam Node - Ramps, Workers, Storehouses
template <typename Node>
class NodeCollection
{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node &&node)
    {
        nodeCollection_.push_back(std::move(node));
    }

    void remove_by_id(ElementID id)
    {
        nodeCollection_.remove_if([id](const Node &node){return node.get_id() == id;});
    }


    [[nodiscard]] const_iterator find_by_id(ElementID id) const
    {
        return std::find_if(nodeCollection_.cbegin(), nodeCollection_.cend(),
                            [id](const Node &node){return node.get_id() == id;});
    }

    iterator find_by_id(ElementID id)
    {
        return std::find_if(nodeCollection_.begin(), nodeCollection_.end(),
                            [id](const Node &node){return node.get_id() == id;});
    }

    iterator begin(){return nodeCollection_.begin();}
    iterator end() {return nodeCollection_.end();}
    [[nodiscard]] const_iterator begin() const {return nodeCollection_.cbegin();}
    [[nodiscard]] const_iterator end() const {return nodeCollection_.cend();}
    [[nodiscard]] const_iterator cbegin() const {return nodeCollection_.cbegin();}
    [[nodiscard]] const_iterator cend() const {return nodeCollection_.cend();}

private:
    container_t nodeCollection_;
};


class Factory
{
public:
    [[nodiscard]] bool is_consistent() const;

    void do_deliveries(Time);

    void do_package_passing();

    void do_work(Time);

    /// Ramp
    void add_ramp(Ramp&& ramp){rampCollection_.add(std::move(ramp));}
    void remove_ramp(ElementID id){rampCollection_.remove_by_id(id);}

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id){return rampCollection_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const{return rampCollection_.find_by_id(id);}

    [[nodiscard]] NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return rampCollection_.cbegin();}
    [[nodiscard]] NodeCollection<Ramp>::const_iterator ramp_cend() const {return rampCollection_.cend();}


    /// Worker
    void add_worker(Worker&& worker){workerCollection_.add(std::move(worker));}
    void remove_worker(ElementID id){remove_receiver(workerCollection_, id);}

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id){return workerCollection_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const{return workerCollection_.find_by_id(id);}

    [[nodiscard]] NodeCollection<Worker>::const_iterator worker_cbegin() const {return workerCollection_.cbegin();}
    [[nodiscard]] NodeCollection<Worker>::const_iterator worker_cend() const {return workerCollection_.cend();}

    /// Storehouse
    void add_storehouse(Storehouse&& storehouse){storehouseCollection_.add(std::move(storehouse));}
    void remove_storehouse(ElementID id){ remove_receiver(storehouseCollection_, id);}

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id){ return storehouseCollection_.find_by_id(id);}
    [[nodiscard]] NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const{return storehouseCollection_.find_by_id(id);}

    [[nodiscard]] NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return storehouseCollection_.cbegin();}
    [[nodiscard]] NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return storehouseCollection_.cend();}

private:
    /// Metoda usuwa element i połączenie
    /// Podczas usunięcia magazynu trzeba usunąc połączenie rampa/worker->magazyn
    template<typename Node>
    void remove_receiver(NodeCollection<Node> &collection, ElementID id);

private:
    NodeCollection<Ramp> rampCollection_;
    NodeCollection<Worker> workerCollection_;
    NodeCollection<Storehouse> storehouseCollection_;
};

Factory load_factory_structure(std::istream&);

void save_factory_structure(Factory&, std::ostream&);


template<typename Node>
void Factory::remove_receiver(NodeCollection<Node> &collection, ElementID id)
{
    /// *iterator = odbiorca, &odbiorca = wskaźnik do odbiorcy
    auto pReciver = &(*collection.find_by_id(id));

    // Dla kazdej dostawcy towaru sprawdzasz czy dostarcza do tego odpbiory
    for (auto &ramp : rampCollection_)
    {
        ramp.receiver_preferences_.remove_receiver(pReciver);
    }

    for (auto &worker : workerCollection_)
    {
        worker.receiver_preferences_.remove_receiver(pReciver);
    }
    collection.remove_by_id(id);
}

#endif //SYMULACJASIECI_FACTORY_HPP
