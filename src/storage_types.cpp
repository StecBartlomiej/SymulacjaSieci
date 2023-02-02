#include "storage_types.hpp"

void PackageQueue::push(Package &&package)
{
    packageList_.emplace_back(std::move(package));
}

Package PackageQueue::pop()
{
    Package deletedPackage;
    switch (packageQueueType_)
    {
        case PackageQueueType::FIFO:
            deletedPackage = std::move(packageList_.front());
            packageList_.pop_front();
            break;

        case PackageQueueType::LIFO:
            deletedPackage = std::move(packageList_.back());
            packageList_.pop_back();
            break;
    }
    return deletedPackage;
}
