#ifndef META_TAGS_HPP
#define META_TAGS_HPP
#include "PlatformHeader.hpp"
#include <string>
namespace YAZF {
    class Meta_Tags {
        public:
            static YAZF_API std::string METADATA_BEGIN;
            static YAZF_API std::string METADATA_END;
            static YAZF_API std::string FILETABLE_BEGIN;
            static YAZF_API std::string FILETABLE_END;
            static YAZF_API std::string PATHTABLE_BEGIN;
            static YAZF_API std::string PATHTABLE_END;
            static YAZF_API std::string FILEINFORMATION_BEGIN;
            static YAZF_API std::string FILEINFORMATION_END;
            static YAZF_API std::string FILECONTENT_BEGIN;
            static YAZF_API std::string FILECONTENT_END;
    };
}
#endif