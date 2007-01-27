
#include <string>

#include "15_env/EnvModule.h"

namespace synthese
{
	namespace env
	{
		class BroadcastScenario : public Registrable<BroadcastScenario, util::uid>
		{
		private:
			std::string									_name;
			EnvModule::LineBroadcastList				_lineAlarmTemplates;
			EnvModule::BroadcastPointBroadcastList		_broadcastPointAlarmBroadcastTemplates;

		public:
			const std::string& getName() const;
		};
	}
}