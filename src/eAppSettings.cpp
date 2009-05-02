#include "eApp.h"

eSettings& eGetSettings(void) {
	return (dynamic_cast<IGetSettings*>(wxTheApp))->GetSettings();
}
