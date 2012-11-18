#include <stdio.h>
#include "sidg_pcmif_server.hpp"
#include "SidPCMInterface.hpp"
#include "SidProtocolEnums.hpp"
#include "sidg_pcmif_cb_client.hpp"
#include "SidDebugging.hpp"

struct OptionsParser {
	OptionsParser() :
		m_IpcPrefix(IPC_PREFIX_DEFAULT),
		m_Help(false) {
	}
	void Usage(const char *executable) {
		SID_INFO("usage:");
		SID_INFO("	%s [options]", executable);
		SID_INFO("	options:");
		SID_INFO("		-h - print help");
		SID_INFO("		-s prefix - path prefix for ipc key. default /tmp/");
	}
	int ParseOptions(int argc, const char **argv) {
		for (int i = 1; i < argc; i++) {
			const char c = argv[i][0];
			if (c == '-' || c == '/') {
				switch (argv[i][1]) {
				case 'h': {
					m_Help = true;
					break;
				}
				case 's': {
					if (i + 1 >= argc)
						return -1;
					m_IpcPrefix = argv[++i];
					break;
				}
				default: {
					return -1;
					break;
				}
				}
			} else {
				return -1;
			}
		}

		return 0;
	}
	const char *m_IpcPrefix;
	bool m_Help;
};


int kit_pcm_host_main(int argc, const char **argv) {
	OptionsParser parser;
	if (parser.ParseOptions(argc, argv) < 0 || parser.m_Help) {
		parser.Usage(argv[0]);
		return -1;
	}

	Sid::SkypePCMInterfaceServer *pcmif_server = new Sid::SkypePCMInterfaceServer();
	Sid::SkypePCMCallbackInterfaceClient *pcmif_cb_client = new Sid::SkypePCMCallbackInterfaceClient();

	SkypePCMInterface* pcmif = SkypePCMInterfaceGet(pcmif_cb_client);
	pcmif_server->set_if(pcmif);


	Sid::String fromskypekitkey;
	Sid::String toskypekitkey;

	fromskypekitkey.Format( "%spcm_from_skypekit_key", parser.m_IpcPrefix);
	toskypekitkey.Format( "%spcm_to_skypekit_key", parser.m_IpcPrefix);

	pcmif_server->Connect(fromskypekitkey.data(), 0);
	pcmif_cb_client->Connect(toskypekitkey.data(), 500);

	Sid::Protocol::Status status;
	do {
		status =pcmif_server->ProcessCommands();
	} while (status == Sid::Protocol::OK);

	SkypePCMInterfaceRelease(pcmif);
	pcmif_server->Disconnect();
	pcmif_cb_client->Disconnect();

	delete pcmif_server;
	delete pcmif_cb_client;

	printf("PCMServerTransport disconnected, exiting from pcmtesthost\n");
}
