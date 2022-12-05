#include "domain.h"

namespace transport_catalogue
{

    namespace units
    {
        // transport router units-------------------------------------

        PathTime operator+(const PathTime lhs, const PathTime rhs)
        {
            PathTime temp;
            temp.time = lhs.time + rhs.time;
            temp.span_count = lhs.span_count + rhs.span_count;
            temp.is_other_bus = true;
            temp.bus = rhs.bus;
            return temp;
        }

        bool operator<(PathTime lhs, PathTime rhs)
        {
            return (lhs.time < rhs.time);
        }

        bool operator>(PathTime lhs, PathTime rhs)
        {
            return !(rhs < lhs);
        }
        bool operator==(PathTime lhs, PathTime rhs)
        {
            return (lhs.time == rhs.time && lhs.span_count == rhs.span_count && lhs.bus == rhs.bus);
        }

        PathTime PathTime::operator=(PathTime other)
        {
            time = other.time;
            bus = other.bus;
            span_count = other.span_count;
            is_other_bus = other.is_other_bus;
            return *this;
        }
        // transport router units-------------------------------------

    } // namespace units

} // namespace transport_catalogue
