

#include <SkypekitVideoTransportClient.hpp>

#define SFT_FOURCC_RGB32 (0x32334942)
#define SFT_FOURCC_UYVY (0x59565955)
#define SFT_FOURCC_YUY2 (0x32595559)


class IOSurfaceTransport : public SkypekitVideoTransportClient {
	public:
		IOSurfaceTransport();
		~IOSurfaceTransport();

		bool Allocate( bufferstruct *b );
		void FreeBuffer( bufferstruct *b );
		bool Present( bufferstruct *b, long long ts );
		virtual bool Present( void *data, bufferstruct *b, long long ts );


};
