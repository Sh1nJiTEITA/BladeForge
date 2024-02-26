#ifndef BF_GUI_H
#define BF_GUI_H

#include <map>
#include <string>
#include <fstream>
#include "implot.h"


//#include "implot_internal.h"

#include "bfLayerHandler.h"
#include "bfCurves3.h"

struct BfGUI {
	bool is_info = true;
};

enum BfEnMenueStatus {
	BF_MENUE_STATUS_INFO_ENABLED = 0x1,
	BF_MENUE_STATUS_INFO_DISABLED = -0x1

};


const std::map<int, std::string> bfSetMenueStr{
	{ 0x1, "Hide perfomance/view pannel"},
	{-0x1, "Show perfomance/view pannel"}
};

std::string bfGetMenueInfoStr(BfGUI gui);

void bfPresentLayerHandler(BfLayerHandler&);


struct Variable {
	std::string name;
	std::vector<int> values;
};

void ShowVariableContents(const Variable& var);
void ShowTestPlot();


#endif // BF_GUI_H
