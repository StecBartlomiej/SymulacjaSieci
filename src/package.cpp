#include "package.hpp"
#include <cassert>

Package::Package()
{
    if (freedIDs_.empty())
    {
        ID_ = assignedIDs_.empty() ? 1 : *assignedIDs_.rbegin() + 1;
    }
    else
    {
        ID_ = *freedIDs_.begin();
        freedIDs_.erase(ID_);
    }

    assignedIDs_.insert(ID_);
}

Package::Package(ElementID id): ID_{id}
{
    if (auto it = assignedIDs_.find(id); it != assignedIDs_.end())
    {
        ID_ = *assignedIDs_.rbegin() + 1;
    }
    assert(assignedIDs_.find(ID_) == assignedIDs_.end());

    assignedIDs_.insert(ID_);

    /// Jeśli użyte ID było w zwolnionych usuń
    freedIDs_.erase(ID_);
}

Package::Package(Package &&other) noexcept: ID_{other.ID_}
{
    other.ID_ = 0;
}

Package::~Package()
{
    if (ID_ != 0)
    {
        assert(assignedIDs_.find(ID_) != assignedIDs_.end());
        assignedIDs_.erase(ID_);
        freedIDs_.insert(ID_);
    }
}

Package &Package::operator=(Package &&other) noexcept
{
    /// Zwolnienie ID przed przypisaniem
    freedIDs_.insert(ID_);
    assignedIDs_.erase(ID_);

    ID_ = other.ID_;
    other.ID_ = 0;
    return *this;
}
