

#include <SkypekitVideoTransportClient.hpp>


class PosixSHMTransport : public SkypekitVideoTransportClient {
	public:
		PosixSHMTransport();
		~PosixSHMTransport();

		bool Allocate( bufferstruct *b );
		void FreeBuffer( bufferstruct *b );
		bool Present( bufferstruct *b, long long ts );

		virtual bool Present( void *data, bufferstruct *b, long long ts );

	private:
		static int bufnum;

		struct bufdata {
			void *mem;
			size_t size;
			int fd;
		};
};
