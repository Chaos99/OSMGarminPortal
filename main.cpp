#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "gdal/ogrsf_frmts.h"

using namespace std;
using namespace boost::filesystem;

struct tile
{
    unsigned int num;
    float coords[4];
};




// load tile information from area.list file
void loadAreas(vector<tile>* tiles_p)
{
    string line;
    stringstream valid_data;
    cout<<current_path()<<endl;
    if (exists("areas.list"))
    {
        ifstream areas_list ("areas.list");
        if (areas_list.is_open())
        {
            int num; //to hold tile number between lines
            while(areas_list.good())
            {
                getline(areas_list, line);
                if(line.find_first_not_of(" \n\t\r\0") != string::npos
                        && line.find_first_of("0123456789") != string::npos
                        && line.find("Generated") == string::npos) // skip empty lines and header
                {

                    if (line[2] != ' ') // the first line with the tile number
                    {
                        num = atoi(line.substr(0,line.find_first_of(':')).c_str());
//                        cout << num << endl;
                    }
                    else      //second line with coords
                    {
                        unsigned int begin = line.find_first_of(':')+2;
                        unsigned int end = line.find(" to");
                        string pair1 = line.substr(begin,end-begin);
                        begin = end +4;
                        string pair2 = line.substr(begin);

                        float s = atof(pair1.substr(0, pair1.find_first_of(',')).c_str());
                        float e = atof(pair1.substr(pair1.find_first_of(',')+1).c_str());
                        float n = atof(pair2.substr(0, pair2.find_first_of(',')).c_str());
                        float w = atof(pair2.substr(pair2.find_first_of(',')+1).c_str());

                        tile t = {num, {s, e, n , w}};

                        tiles_p->push_back(t);

//                       cout.precision(8);
//                       cout << line << endl;
//                       cout << pair1 << "####"<< pair2 << endl;
//                       cout << s << "##" << e << "##" <<endl;
//                       cout << n << "##" << w << "##" <<endl;
                    }
                }
            }
        }
    }
    else
    {
        cout << "file not found " << endl;
    }
}

int main()
{
    vector<tile> tiles;
    vector<string> availShapes;

    loadAreas(&tiles);

//    for (unsigned int i = 0; i < tiles.size(); i++)
//    {
//        cout << tiles[i].num <<": " << tiles[i].coords[0]<< endl;
//    }

    OGRRegisterAll();

    OGRDataSource * poDS;

    char *path = "../../admin_level_6/shape_al6/admin_level_6.shp";

    if (exists(path))
    {
        poDS = OGRSFDriverRegistrar::Open(path);
        if (poDS == NULL)
        {
            cout << "Data read error" << endl;
            exit (1);
        }
    }
    else
    {
        cout << "File not found" << endl;
        exit(1);
    }

    cout << "Found " << poDS->GetLayerCount() << "Layers in File" << endl;
    OGRLayer *poLayer;
    OGRFeature *poFeature;
    poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();
    while ( (poFeature = poLayer->GetNextFeature()) != NULL)
    {
//        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
//        int iField;
//
//        for (iField = 0; iField < poFDefn->GetFieldCount(); iField++)
//        {
//            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(0);
        availShapes.push_back(poFeature->GetFieldAsString(1));
//            cout << poFeature->GetFieldAsString(1) << endl;
//        }
        OGRFeature::DestroyFeature(poFeature);
    }


    string selected = "";
    bool found = false;

    while (!found)
    {
        cout << "Select a shape: ";
        cin >> selected;
        for (unsigned int i = 0; i < availShapes.size(); i++)
        {
            if (availShapes[i].find(selected) != string::npos)
            {
                found = true;
                cout << "Found match: " << availShapes[i] << endl;
                selected = availShapes[i];
            }
        }
    }

    poLayer->ResetReading();
    while ( (poFeature = poLayer->GetNextFeature()) != NULL)
    {
        cout<<'.';
        if (string(poFeature->GetFieldAsString(1)).find(selected) != string::npos)
        {
            cout<<endl;
            OGRGeometry *poGeometry;

            poGeometry = poFeature->GetGeometryRef();
            cout << poGeometry->getGeometryType()<< endl;

            if (poGeometry != NULL
                    && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
            {
                OGRPolygon *poPoly = (OGRPolygon *) poGeometry;
                OGREnvelope *poEnv = new OGREnvelope();
                poPoly->getEnvelope(poEnv);
//                ofstream outfile ("outfile.kml");
                double s = poEnv->MaxY;
                double n = poEnv->MinY;
                double e = poEnv->MaxX;
                double w = poEnv->MinX;

                cout<< s << ", " << n << " to " << e << ", " << w << endl;

//                outfile.close();
//                    cout << poPoint->getX() <<", "<<poPoint->getY()<<endl;
            }
            else
            {
                cout << "No point Geometry"<<endl;
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }
//todo:
// get boundary rectangle of selected shape
// pre- sort tiles
// check remaining tiles for inclusion in shape

    OGRDataSource::DestroyDataSource(poDS);
//cout << valid_data.str()<<endl  ;
    return 0;
}




