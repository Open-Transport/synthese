
#include <vector>

#include "Registrable.h"

#include "04_time/DateTime.h"

#include "15_env/EnvModule.h"

namespace synthese
{
	namespace env
	{
		class BroadcastScenario;
		class LineAlarmBroadcast;
		class BroadcastPointAlarmBroadcast;

		class ScenarioBroadcast : public util::Registrable
		{
		private:
			time::DateTime							_start;
			time::DateTime							_end;
			BroadcastScenario*						_template;
			EnvModule::LineBroadcastList			_lineAlarms;
			EnvModule::BroadcastPointBroadcastList	_broadcastPointAlarmBroadcasts;

		public:
			ScenarioBroadcast();
			~ScenarioBroadcast();

		};
	}
}