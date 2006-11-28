
#ifndef SYNTHESE_VinciBike_H__
#define SYNTHESE_VinciBike_H__

namespace synthese
{
	namespace vinci
	{
		class VinciBike : public util::Registrable<uid, VinciBike>
		{
		public:
			std::string _markedNumber;
		};
	}
}

#endif // SYNTHESE_VinciBike_H__
