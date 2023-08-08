#include <Config.hpp>

Config::Config( Module *_mainMod ) : mainMod(_mainMod)
{
	vector<Module*> mainSubMods = mainMod->getSubMods();

	for (size_t i = 0; i < mainSubMods.size(); i++)
	{
		Module * subMod = mainSubMods[i];

		if (subMod->getName() == "server")
		{
			ServerModule *srvMod = dynamic_cast<ServerModule *>(subMod);
			// check casting
			srvConfs.push_back(ServerConfig(*srvMod));
		}
		// else if (subMod->getName() == "worker_connections")
		// {
		// 	WorkerConnectionsModule *m = dynamic_cast<WorkerConnectionsModule *>(subMod);
		// 	// check casting
		// 	mainConf.workConnMod = m;
		// }
		
	}
}

