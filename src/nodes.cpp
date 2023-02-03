#include "nodes.hpp"

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d): id_{id}, pStockpile_(std::move(d)) {}

void ReceiverPreferences::add_receiver(IPackageReceiver *packageReceiver)
{
    preferences_.emplace(packageReceiver, 2);
    reassign_probability();
}

void ReceiverPreferences::reassign_probability()
{
    /// Dla każdego równe prawdopodobieństwo
    double probability = 1.f / static_cast<double>(preferences_.size());
    for (auto& [key, value] : preferences_)
    {
        value = probability;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *packageReceiver)
{
    preferences_.erase(packageReceiver);
    reassign_probability();
}

IPackageReceiver *ReceiverPreferences::choose_receiver()
{
    if (!preferences_.empty())
    {
        double random_number = probabilityGenerator_();
        double sum_probability = 0;

        for (auto [key, value]: preferences_)
        {
            sum_probability += value;
            if (random_number < sum_probability) {return key;}
        }
    }
    return nullptr;
}


void PackageSender::push_package(Package &&package)
{
    buffer_.emplace(std::move(package));
}

void PackageSender::send_package()
{
    if (buffer_)
    {
        receiver_preferences_.choose_receiver()->receive_package(std::move(buffer_.value()));
        buffer_.reset();
    }
}

void Ramp::deliver_goods(Time t)
{
    if (t % timeOffset_ == 0) /// Only if time increases by 1!
    {
        send_package();
    }
    else if(!get_sending_buffer().has_value())
    {
        push_package(Package());
    }
}

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q): id_{id}, timeOffset_{pd},
                                                                                packageQueue_(std::move(q))
{

}

void Worker::do_work(Time t)
{
    if (!processing_buffer_.has_value())
    {
        processing_buffer_ = packageQueue_->pop();
        processingStartTime_ = t;
    }
    if (t % timeOffset_ == 0)
    {
        push_package(std::move(processing_buffer_.value()));
    }

}

void Worker::receive_package(Package &&package)
{
    packageQueue_->push(std::move(package));
}
