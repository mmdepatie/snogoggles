#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>
#include <G4RotationMatrix.hh>
#include <G4Point3D.hh>
#include <G4Polyhedron.hh>

#include <stdlib.h>
#include <sstream>
using namespace std;

#include <TMath.h>

#include <Viewer/GDMLLoader3d.hh>
#include <Viewer/ColourPalette.hh>
#include <Viewer/World.hh>

namespace Viewer {
namespace Frames {

int GDMLLoader3d::fColour = 0;

World* GDMLLoader3d::GetWorld() 
{
    G4GDMLParser parser;
    stringstream parserFileName;
    parserFileName << getenv( "VIEWERROOT" ) << "/data/";
    parser.Read( parserFileName.str() );

    Volume volume = CreateVolume( parser.GetWorldVolume() );
//    VisMap visMap;
//    CreateVisMap( visMap, volume );
//    return new World( volume, visMap );
    return new World( volume );
}


Volume GDMLLoader3d::CreateVolume( G4VPhysicalVolume* vol )
{
    G4LogicalVolume* log = vol->GetLogicalVolume();
    G4VSolid* solid = log->GetSolid();

    G4ThreeVector pos = vol->GetTranslation();

    G4ThreeVector axis;
    double rotationAngle = 0;
    G4RotationMatrix* rotation = vol->GetRotation();
    if( rotation != NULL )
    {
        axis = rotation->getAxis();
        rotationAngle = rotation->getDelta() * 180 / TMath::Pi();
    }

    Volume volume(
        log->GetName(),
        Vector3( pos.x(), pos.y(), pos.z() ),
        Vector3( axis.x(), axis.y(), axis.z() ),
        rotationAngle,
        CreatePolyhedron( solid->CreatePolyhedron() )
    );

    for( int i = 0; i < log->GetNoDaughters(); i++ )
    {
        Volume daughter = CreateVolume( log->GetDaughter(i) );
        volume.AddDaughter( daughter );
    }

    return volume;
}

Polyhedron GDMLLoader3d::CreatePolyhedron( G4Polyhedron *pol )
{
    Polyhedron polyhedron;
    bool notLastFace = true;

    while( notLastFace == true )
    {
        int n;
        G4Point3D nodes[4];
        notLastFace = pol->GetNextFacet( n, nodes );

        Polygon polygon;
        polygon.AddVertex( Vector3( nodes[0].x(), nodes[0].y(), nodes[0].z() ) );
        polygon.AddVertex( Vector3( nodes[1].x(), nodes[1].y(), nodes[1].z() ) );
        polygon.AddVertex( Vector3( nodes[2].x(), nodes[2].y(), nodes[2].z() ) );

        if( nodes[3].distance() > 0.001 )
            polygon.AddVertex( Vector3( nodes[3].x(), nodes[3].y(), nodes[3].z() ) );

        polyhedron.AddPolygon( polygon );
    }

    return polyhedron;
}

}; // namespace Frames
}; // namespace Viewer
