#ifndef SYMULACJASIECI_PACKAGE_HPP
#define SYMULACJASIECI_PACKAGE_HPP

#include "types.hpp"
#include <set>

// uncomment to disable assert()
#define NDEBUG

class Package
{
public:
    Package();
    explicit Package(ElementID);
    Package(Package&&) noexcept;
    ~Package();

    Package& operator=(Package&&) noexcept;

    [[nodiscard]] ElementID get_id() const{ return ID_;};

private:
    ElementID ID_;
    static inline std::set<ElementID> assignedIDs_{};
    static inline std::set<ElementID> freedIDs_{};
};


#endif //SYMULACJASIECI_PACKAGE_HPP
