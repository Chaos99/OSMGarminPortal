#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "gdal/ogrsf_frmts.h"

#include "Tile.h"


using namespace std;
using namespace boost::filesystem;


// load tile information from area.list file
void loadAreas(vector<Tile*>* tiles_p)
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

                        Tile* t = new Tile(s, e, n , w, num);

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

void getAvailShapes(vector<OGRFeature*>* availShapes_p, const string selected = "")
{

    OGRRegisterAll();

    OGRDataSource * poDS;

    string path = "../admin_level_6/shape_al6/admin_level_6.shp";

    if (exists(path))
    {
        poDS = OGRSFDriverRegistrar::Open(path.c_str());
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

//    cout << "Found " << poDS->GetLayerCount() << "Layers in File" << endl;
    OGRLayer *poLayer;
    OGRFeature *poFeature;
    poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();
    while ( (poFeature = poLayer->GetNextFeature()) != NULL)
    {
        if (selected.empty())
        {
            availShapes_p->push_back(poFeature);
        }
        else
        {
            if (string(poFeature->GetFieldAsString(1)).find(selected) != string::npos)
                availShapes_p->push_back(poFeature);
            else
                OGRFeature::DestroyFeature(poFeature);
        }
    }

    OGRDataSource::DestroyDataSource(poDS);
}

OGREnvelope* getBBoxOfShape(OGRFeature* poFeature)
{

    OGRGeometry *poGeometry;

    poGeometry = poFeature->GetGeometryRef();

    cout <<"extracting shape of " << poFeature->GetFieldAsString(1) << endl;

    if (poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
    {
        OGRPolygon *poPoly = (OGRPolygon *) poGeometry;
        OGREnvelope *poEnv = new OGREnvelope();

        poPoly->getEnvelope(poEnv);
        return poEnv;
//                ofstream outfile ("outfile.kml");
//                double s = poEnv->MaxY;
//                double n = poEnv->MinY;
//                double e = poEnv->MaxX;
//                double w = poEnv->MinX;
//
//                cout<< s << ", " << n << " to " << e << ", " << w << endl;

//                outfile.close();
//                    cout << poPoint->getX() <<", "<<poPoint->getY()<<endl;
    }
    else
    {
        cout << "No point Geometry"<<endl;
        return NULL;
    }

}

vector<Tile*>* tilesInBBox(vector<Tile*>* tiles, OGREnvelope* bbox)
{


}


int main()
{
    vector<Tile*> tiles;
    vector<OGRFeature*> availShapes;

    loadAreas(&tiles);

    OGREnvelope* bbox;

//    for (unsigned int i = 0; i < tiles.size(); i++)
//    {
//        cout << tiles[i].num <<": " << tiles[i].coords[0]<< endl;
//    }

    string selected = "";
    bool found = false;

    while (!found)
    {
        selected.clear();
        cout << "Select a shape: ";
        cin >> selected;

        getAvailShapes(&availShapes, selected);

        if (availShapes.size() >1)
        {
            //found = true;
            cout << "Found more than one match: " << endl;
            for (unsigned int i = 0; i < availShapes.size(); i++)
            {
                cout << "\t" << availShapes[i]->GetFieldAsString(1) << endl;
            }
            cout << "(Please be more specific.)" << endl;
            // cleanup
            for (unsigned int i = 0; i < availShapes.size(); i++)
            {
                OGRFeature::DestroyFeature(availShapes[i]);

            }
            availShapes.clear();

        } else if (availShapes.size() == 1)
        {
            found = true;
            cout << "Found match: " << availShapes[0]->GetFieldAsString(1) << endl;
        }
        else
            cout << "No match found, try again." << endl;
    }

    bbox = getBBoxOfShape(availShapes[0]);


    // cleanup
    for (unsigned int i = 0; i < availShapes.size(); i++)
            {
                OGRFeature::DestroyFeature(availShapes[i]);
            }

    return 0;
}




//todo:
// get boundary rectangle of selected shape
// pre- sort tiles
// check remaining tiles for inclusion in shape





