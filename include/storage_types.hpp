#ifndef SYMULACJASIECI_STORAGE_TYPES_HPP
#define SYMULACJASIECI_STORAGE_TYPES_HPP

#include <list>
#include "package.hpp"

enum class PackageQueueType
{
    FIFO, LIFO
};


class IPackageStockpile
{
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual ~IPackageStockpile() = default;

    virtual void push(Package&&) = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;
};


class IPackageQueue: public IPackageStockpile
{
public:
    virtual ~IPackageQueue() = default;
    virtual Package pop() = 0;
    [[nodiscard]] virtual PackageQueueType get_queue_type() const = 0;
};


class PackageQueue final: public IPackageQueue
{
public:
    explicit PackageQueue(PackageQueueType packageQueueType): packageQueueType_{packageQueueType} {}

    void push(Package&&) override;
    [[nodiscard]] bool empty() const override {return packageList_.empty();}
    [[nodiscard]] std::size_t size() const override {return  packageList_.size();}
    Package pop() override;
    [[nodiscard]] PackageQueueType get_queue_type() const override {return packageQueueType_;}

    [[nodiscard]] const_iterator begin() const override {return packageList_.cbegin();}
    [[nodiscard]] const_iterator cbegin() const override {return packageList_.cbegin();}
    [[nodiscard]] const_iterator end() const override {return packageList_.cend();}
    [[nodiscard]] const_iterator cend() const override {return packageList_.cend();}

private:
    std::list<Package> packageList_;
    PackageQueueType packageQueueType_;
};

#endif //SYMULACJASIECI_STORAGE_TYPES_HPP
