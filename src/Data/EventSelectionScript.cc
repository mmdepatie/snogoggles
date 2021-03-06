#include <Python.h>

#include <sstream>
using namespace std;

#include <Viewer/EventSelectionScript.hh>
using namespace Viewer;
#include <Viewer/RIDS/Event.hh>

EventSelectionScript::EventSelectionScript()
{
  stringstream pythonScriptPath;
  pythonScriptPath << getenv( "VIEWERROOT" ) << "/scripts/eventselect";
  PyObject* pSysPath = PySys_GetObject( "path" );
  PyObject* pPath = PyString_FromString( pythonScriptPath.str().c_str() );
  PyList_Append( pSysPath, pPath );
  //Py_DECREF( pSysPath );
  Py_DECREF( pPath );
  fpScript = NULL;
  fpSelectFunction = NULL;
}

EventSelectionScript::~EventSelectionScript()
{
  UnLoad();
}

void
EventSelectionScript::UnLoad()
{
  Py_XDECREF( fpScript ); // Delete old script if exists
  Py_XDECREF( fpSelectFunction );
}

void
EventSelectionScript::Load( const string& scriptName )
{
  stringstream scriptPath;
  scriptPath << "evselect_" << scriptName;
  fCurrentScript = scriptPath.str();
  UnLoad();
  // Load new script
  PyObject* pScriptName = PyString_FromString( fCurrentScript.c_str() );
  fpScript = PyImport_Import( pScriptName ); // Load script
  Py_DECREF( pScriptName );
  fpSelectFunction = PyObject_GetAttrString( fpScript, "select" );
  if( !fpSelectFunction || !PyCallable_Check( fpSelectFunction ) )
    throw;
}

bool
EventSelectionScript::ProcessEvent( const RIDS::Event& event )
{
  /// Build data to send to the script
  PyObject* pDataDict = PyDict_New();
  PyObject* pTrigger = PyInt_FromLong( event.GetTrigger() );
  PyDict_SetItemString( pDataDict, "trigger", pTrigger );
  Py_DECREF( pTrigger );
  PyObject* pNhit = PyInt_FromLong( 0 );
  PyDict_SetItemString( pDataDict, "nhit", pNhit );
  Py_DECREF( pNhit );

  /// Call the script, check the result and return true/false
  PyObject* fpInput = PyString_FromString( fInputString.c_str() );
  bool result = false;
  PyObject* pResult = PyObject_CallFunctionObjArgs( fpSelectFunction, pDataDict, fpInput, NULL );
  if( pResult == Py_True )
    result = true;
  Py_XDECREF( pResult );
  Py_DECREF( pDataDict );
  Py_DECREF( fpInput );
  return result;
  return true;
}
