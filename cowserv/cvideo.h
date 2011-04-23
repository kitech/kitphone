
#ifndef CVideo_INCLUDED_HPP
#define CVideo_INCLUDED_HPP

#include "skype-embedded_2.h"
#include "skype-object.h"

class CVideo : public Video
{
public:
        typedef DRef<CVideo, Video> Ref;
        CVideo(unsigned int oid, SERootObject* root);
        ~CVideo() {}

        void OnChange(int prop);
};

#endif //CVideo_INCLUDED_HPP

