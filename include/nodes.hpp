#ifndef SYMULACJASIECI_NODES_HPP
#define SYMULACJASIECI_NODES_HPP

#include "storage_types.hpp"
#include "types.hpp"
#include "helpers.hpp"
#include "config.hpp"

#include <memory>
#include <map>
#include <optional>
#include <utility>


//#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
enum class ReceiverType
{
    WORKER, STOREHOUSE
};
//#endif

class IPackageReceiver
{
public:
    virtual ~IPackageReceiver() = default;

    virtual void receive_package(Package&&) = 0;

    virtual ElementID get_id() const = 0;

    // Dopiero w zadaniu Fabryka!
//#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    [[nodiscard]] virtual ReceiverType get_receiver_type() const = 0;
//#endif

    [[nodiscard]] virtual IPackageStockpile::const_iterator begin() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator cbegin() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator end() const = 0;
    [[nodiscard]] virtual IPackageStockpile::const_iterator cend() const = 0;
};


class Storehouse final: public IPackageReceiver
{
public:
    explicit Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::LIFO));

    [[nodiscard]] ElementID get_id() const override {return id_;}

    // Dopiero w zadaniu Fabryka!
//#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    [[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE;}
//#endif

    void receive_package(Package &&p) override {pStockpile_->push(std::move(p));}

    [[nodiscard]] IPackageStockpile::const_iterator begin() const override {return pStockpile_->begin();}
    [[nodiscard]] IPackageStockpile::const_iterator cbegin() const override {return pStockpile_->cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator end() const override {return pStockpile_->end();}
    [[nodiscard]] IPackageStockpile::const_iterator cend() const override {return pStockpile_->cend();}

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> pStockpile_;
};


class ReceiverPreferences
{
public:
    using preferences_t = std::map<IPackageReceiver*, double>; /// Receiver : probability
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator): probabilityGenerator_{std::move(pg)} {}

    void add_receiver(IPackageReceiver*);
    void remove_receiver(IPackageReceiver*);
    IPackageReceiver* choose_receiver();

    [[nodiscard]] const preferences_t& get_preferences() const {return preferences_;}

    [[nodiscard]] const_iterator begin() const {return preferences_.cbegin();}
    [[nodiscard]] const_iterator cbegin() const {return preferences_.cbegin();}
    [[nodiscard]] const_iterator end() const {return preferences_.cend();}
    [[nodiscard]] const_iterator cend() const {return preferences_.cend();}

public:
    preferences_t preferences_;
    ProbabilityGenerator probabilityGenerator_;

private:
    void reassign_probability();
};


class PackageSender
{
public:
    PackageSender() = default;
    PackageSender(PackageSender&&) = default;

    void send_package();

    [[nodiscard]] const std::optional<Package>& get_sending_buffer() const {return buffer_;}

protected:
    void push_package(Package&&);

public:
    ReceiverPreferences receiver_preferences_;

private:
    std::optional<Package> buffer_;
};


class Ramp final: public PackageSender
{
public:
    Ramp(ElementID id, TimeOffset di): id_{id}, timeOffset_{di} {push_package(Package());}

    void deliver_goods(Time t);

    [[nodiscard]] TimeOffset get_delivery_interval() const {return timeOffset_;}

    [[nodiscard]] ElementID get_id() const {return id_;}

private:
    ElementID id_;
    TimeOffset timeOffset_;
};


class Worker final: public PackageSender, public IPackageReceiver
{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

//#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    [[nodiscard]] ReceiverType get_receiver_type() const override {return ReceiverType::WORKER;}
//#endif

    [[nodiscard]] IPackageQueue* get_queue() const {return packageQueue_.get();}

    void receive_package(Package&&) override;

    [[nodiscard]] ElementID get_id() const override {return id_;}

    void do_work(Time t);

    [[nodiscard]] TimeOffset get_processing_duration() const {return timeOffset_;}

    [[nodiscard]] Time get_package_processing_start_time() const {return processingStartTime_;}

    [[nodiscard]] const std::optional<Package>& get_processing_buffer() const {return processing_buffer_;}

    [[nodiscard]] IPackageStockpile::const_iterator begin() const override {return packageQueue_->begin();}
    [[nodiscard]] IPackageStockpile::const_iterator cbegin() const override {return packageQueue_->cbegin();}
    [[nodiscard]] IPackageStockpile::const_iterator end() const override {return packageQueue_->end();}
    [[nodiscard]] IPackageStockpile::const_iterator cend() const override {return packageQueue_->cend();}

private:
    ElementID id_;
    TimeOffset timeOffset_;
    Time processingStartTime_ = 0;
    std::unique_ptr<IPackageQueue> packageQueue_;
    std::optional<Package> processing_buffer_;
};

#endif //SYMULACJASIECI_NODES_HPP
