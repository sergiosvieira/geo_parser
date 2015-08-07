#include <iostream>
#include "geo_parser.h"

int main()
{
	std::string input = "PROJCS[\"UTM Zone 24, Southern Hemisphere\",GEOGCS[\"WGS84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-39],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",10000000],UNIT[\"Meter\",1],AUTHORITY[\"EPSG\",\"32724\"]]";
	ParserToken root;
	ParserError result = geo_parser(input, root);
    ParserToken* datum = find(&root, "UNIT");
    if (datum != nullptr)
    {
        std::cout << datum->label << "\n";
    }
    else
    {
        std::cout << "Value not found\n";
    }
    return 0;
}

