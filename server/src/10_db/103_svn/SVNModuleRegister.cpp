
#include "SVNModule.inc.cpp"

#include "SVNModule.hpp"

#include "SVNCheckoutAction.hpp"
#include "SVNCommitAction.hpp"
#include "SVNUpdateAction.hpp"
#include "SVNWorkingCopyCreateAction.hpp"

void synthese::db::svn::moduleRegister()
{
	synthese::db::svn::SVNModule::integrate();

	synthese::db::svn::SVNCheckoutAction::integrate();
	synthese::db::svn::SVNCommitAction::integrate();
	synthese::db::svn::SVNUpdateAction::integrate();
	synthese::db::svn::SVNWorkingCopyCreateAction::integrate();
}
