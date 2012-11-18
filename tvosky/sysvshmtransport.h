

#include <SkypekitVideoTransportClient.hpp>


class SysVSHMTransport : public SkypekitVideoTransportClient {
	public:
		SysVSHMTransport();
		~SysVSHMTransport();

		bool Allocate( bufferstruct *b );
		void FreeBuffer( bufferstruct *b );
		bool Present( bufferstruct *b, long long ts );

		virtual bool Present( void *data, bufferstruct *b, long long ts );

};
