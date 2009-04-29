#include "eApp.h"

eSettings& eGetSettings(void) {
	return (static_cast<IGetSettings*>((eApp*)wxTheApp))->GetSettings();
}
