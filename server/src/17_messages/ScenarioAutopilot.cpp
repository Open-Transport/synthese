#include "ScenarioAutopilot.hpp"
#include "SentScenarioDao.hpp"
#include "SentScenario.h"

#include <boost/foreach.hpp>

namespace synthese
{
namespace messages
{


ScenarioAutopilot::ScenarioAutopilot(const SentScenarioDao& sentScenarioDao,
									 const util::Clock& clock)
	: _sentScenarioDao(sentScenarioDao)
	, _clock(clock)
{

}


ScenarioAutopilot::~ScenarioAutopilot()
{


}


void
ScenarioAutopilot::runOnce() const
{
	boost::posix_time::ptime now(_clock.getLocalTime());
	std::vector<boost::shared_ptr<SentScenario> > sentScenarios(_sentScenarioDao.list());
	BOOST_FOREACH(boost::shared_ptr<SentScenario> sentScenario, sentScenarios)
	{
		if ((!sentScenario->belongsToAnAutomaticSection() ||
			(sentScenario->getArchived()))) continue;

		bool shouldBeEnabled(sentScenario->shouldBeEnabled(now));
		bool needSaving(false);
		if (shouldBeEnabled != sentScenario->getIsEnabled())
		{
			sentScenario->setIsEnabled(shouldBeEnabled);
			needSaving = true;
		}
		if (sentScenario->shouldBeArchived(now))
		{
			sentScenario->setArchived(true);
			needSaving = true;
		}
		if (needSaving)	_sentScenarioDao.save(sentScenario);
	}
}


}
}
