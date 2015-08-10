/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef MOOSEAPP_H
#define MOOSEAPP_H

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "Moose.h"
#include "Parser.h"
#include "MooseSyntax.h"
#include "ActionWarehouse.h"
#include "SystemInfo.h"
#include "Factory.h"
#include "ActionFactory.h"
#include "OutputWarehouse.h"
#include "InputParameterWarehouse.h"
#include "RestartableData.h"

// libMesh includes
#include "libmesh/parallel_object.h"

class Executioner;
class MooseApp;
class RecoverBaseAction;
class Backup;
class FEProblem;

template<>
InputParameters validParams<MooseApp>();

/**
 * Base class for MOOSE-based applications
 *
 * This generic class for application provides:
 * - parsing command line arguments,
 * - parsing an input file,
 * - executing the simulation
 *
 * Each application should register its own objects and register its own special syntax
 */
class MooseApp : public libMesh::ParallelObject
{
public:
  virtual ~MooseApp();

  /**
   * Get the name of the object
   * @return The name of the object
   */
  const std::string & name() { return _name; }

  /**
   * Get the parameters of the object
   * @return The parameters of the object
   */
  InputParameters & parameters() { return _pars; }

  /**
   * Get the type of this object as a string
   * @return The the type of the object
   */
  const std::string & type() const { return _type; }

  ///@{
  /**
   * Retrieve a parameter for the object
   * @param name The name of the parameter
   * @return The value of the parameter
   */
  template <typename T>
  const T & getParam(const std::string & name);

  template <typename T>
  const T & getParam(const std::string & name) const;
  ///@}

  inline bool isParamValid(const std::string &name) const { return _pars.isParamValid(name); }

  /**
   * Run the application
   */
  virtual void run();

  /**
   * Setup options based on InputParameters.
   */
  virtual void setupOptions();

  ActionWarehouse & actionWarehouse() { return _action_warehouse; }

  Parser & parser() { return _parser; }

  Syntax & syntax() { return _syntax; }

  /**
   * Set the input file name.
   */
  void setInputFileName(std::string input_file_name);

  /**
   * Returns the input file name that was set with setInputFileName
   */
  std::string getInputFileName(){ return _input_filename; }

  /**
   * Override the selection of the output file base name.
   */
  void setOutputFileBase(std::string output_file_base) { _output_file_base = output_file_base; }

  /**
   * Override the selection of the output file base name.
   */
  std::string getOutputFileBase();

  /**
   * Tell the app to output in a specific position.
   */
  void setOutputPosition(Point p);

  /**
   * Whether or not an output position has been set.
   * @return True if it has
   */
  bool hasOutputPosition() { return _output_position_set; }

  /**
   * Get the output position.
   * @return The position offset for the output.
   */
  Point getOutputPosition() { return _output_position; }

  /**
   * Set the starting time for the simulation.  This will override any choice
   * made in the input file.
   *
   * @param time The start time for the simulation.
   */
  void setStartTime(const Real time) { _start_time_set = true; _start_time = time; }

  /**
   * @return Whether or not a start time has been programmatically set using setStartTime()
   */
  bool hasStartTime() { return _start_time_set; }

  /**
   * @return The start time
   */
  Real getStartTime() { return _start_time; }

  /**
   * Each App has it's own local time.  The "global" time of the whole problem might be
   * different.  This offset is how far off the local App time is from the global time.
   */
  void setGlobalTimeOffset(const Real offset) { _global_time_offset = offset; }

  /**
   * Each App has it's own local time.  The "global" time of the whole problem might be
   * different.  This offset is how far off the local App time is from the global time.
   */
  Real getGlobalTimeOffset() { return _global_time_offset; }

  /**
   * Return the filename that was parsed
   */
  std::string getFileName(bool stripLeadingPath = true) const;

  /**
   * Set a flag so that the parser will either warn or error when unused variables are seen after
   * parsing is complete.
   */
  void setCheckUnusedFlag(bool warn_is_error = false);

  /**
   * Set a flag so that the parser will throw an error if overridden parameters are detected
   */
  void setErrorOverridden();

  /**
   * Removes warnings and error checks for unrecognized variables in the input file
   */
  void disableCheckUnusedFlag();

  /**
   * Tell MOOSE to compute all aux kernels when any user objects are computed - deprecated behavior
   */
  bool & legacyUoAuxComputationDefault();

  /**
   * Tell MOOSE to compute all aux kernels when any user objects are computed - deprecated behavior
   */
  bool & legacyUoInitializationDefault();

  /**
   * Retrieve the Executioner for this App
   */
  Executioner * getExecutioner() { return _executioner.get(); }

  /**
   * Retrieve the Executioner shared pointer for this App
   */
  MooseSharedPointer<Executioner> & executioner() { return _executioner; }

  /**
   * Set a Boolean indicating whether this app will use a Nonlinear or Eigen System.
   */
  bool & useNonlinear() { return _use_nonlinear; }

  /**
   * Retrieve the Factory associated with this App.
   */
  Factory & getFactory() { return _factory; }

  /**
   * Retrieve the ActionFactory associated with this App.
   */
  ActionFactory & getActionFactory() { return _action_factory; }

  /**
   * Get the command line
   * @return The reference to the command line object
   * Setup options based on InputParameters.
   */
  MooseSharedPointer<CommandLine> commandLine() { return _command_line; }

  /**
   * This method is here so we can determine whether or not we need to
   * use a separate reader to read the mesh BEFORE we create the mesh.
   */
  bool & setFileRestart() { return _initial_from_file; }

  /**
   * Actually build everything in the input file.
   */
  virtual void runInputFile();

  /**
   * Execute the Executioner that was built.
   */
  virtual void executeExecutioner();

  /**
   * Returns true if the user specified --parallel-mesh on the command line and false
   * otherwise.
   */
  bool getParallelMeshOnCommandLine() const { return _parallel_mesh_on_command_line; }

  /**
   * Whether or not this is a "recover" calculation.
   */
  bool isRecovering() const { return _recover; }

  /**
   * Whether or not this is a "recover" calculation.
   */
  bool isRestarting() const { return _restart; }

  /**
   * Return true if the recovery file base is set
   */
  bool hasRecoverFileBase() { return !_recover_base.empty(); }

  /**
   * The file_base for the recovery file.
   */
  std::string getRecoverFileBase() { return _recover_base; }

  /**
   * mutator for recover_base (set by RecoverBaseAction)
   */
  void setRecoverFileBase(std::string recover_base) { _recover_base = recover_base; }

  /**
   *  Whether or not this simulation should only run half its transient (useful for testing recovery)
   */
  bool halfTransient() { return _half_transient; }

  /**
   * Store a map of outputter names and file numbers
   * The MultiApp system requires this to get the file numbering to propagate down through the Multiapps.
   * @param numbers Map of outputter names and file numbers
   *
   * @see MultiApp TransientMultiApp OutputWarehouse
   */
  void setOutputFileNumbers(std::map<std::string, unsigned int> numbers) { _output_file_numbers = numbers; }

  /**
   * Store a map of outputter names and file numbers
   * The MultiApp system requires this to get the file numbering to propogate down through the
   * multiapps.
   *
   * @see MultiApp TransientMultiApp
   */
  std::map<std::string, unsigned int> & getOutputFileNumbers() { return _output_file_numbers; }

  /**
   * Return true if the output position has been set
   */
  bool hasOutputWarehouse(){ return _output_position_set; }

  /**
   * Get the OutputWarehouse objects
   */
  OutputWarehouse & getOutputWarehouse();

  /**
   * Get SystemInfo object
   * @return A pointer to the SystemInformation object
   */
  SystemInfo * getSystemInfo() { return _sys_info.get(); }

  ///@{
  /**
   * Thes methods are called to register applications or objects on demand. This method
   * attempts to load a dynamic library and register it when it is needed. Throws an error if
   * no suitable library is found that contains the app_name in question.
   */
  void dynamicObjectRegistration(const std::string & app_name, Factory * factory, std::string library_path);
  void dynamicAppRegistration(const std::string & app_name, std::string library_path);
  void dynamicSyntaxAssociation(const std::string & app_name, Syntax * syntax, ActionFactory * action_factory, std::string library_path);
  ///@}

  /**
   * Converts an application name to a library name:
   * Examples:
   *   AnimalApp -> libanimal-oprof.la (assuming METHOD=oprof)
   *   ThreeWordAnimalApp -> libthree_word_animal-dbg.la (assuming METHOD=dbg)
   */
  std::string appNameToLibName(const std::string & app_name) const;

  /**
   * Converts a library name to an application name:
   */
  std::string libNameToAppName(const std::string & library_name) const;

  /**
   * Return the loaded library filenames in a std::vector
   */
  std::set<std::string> getLoadedLibraryPaths() const;

  /**
   * Get the InputParameterWarehouse for MooseObjects
   */
  InputParameterWarehouse & getInputParameterWarehouse();

  /**
   * Returns true if legacy constructors are being used
   */
  bool usingLegacyConstructors() { return _legacy_constructors; }

  /*
   * Register a piece of restartable data.  This is data that will get
   * written / read to / from a restart file.
   *
   * @param name The full (unique) name.
   * @param data The actual data object.
   * @param tid The thread id of the object.  Use 0 if the object is not threaded.
   */
  virtual void registerRestartableData(std::string name, RestartableDataValue * data, THREAD_ID tid);

  /**
   * Return reference to the restatable data object
   * @return A const reference to the restatable data object
   */
  const RestartableDatas & getRestartableData() { return _restartable_data; }

  /**
   * Return a reference to the recoverable data object
   * @return A const reference to the recoverable data
   */
  std::set<std::string> & getRecoverableData() { return _recoverable_data; }

  /**
   * Create a Backup from the current App.  A Backup contains all the data necessary to be able
   * to restore the state of an App.
   */
  MooseSharedPointer<Backup> backup();

  /**
   * Restore a Backup.  This sets the App's state.
   */
  void restore(MooseSharedPointer<Backup> backup);

  /**
   * Returns a string to be printed at the beginning of a simulation
   */
  virtual std::string header() const;

protected:

  /**
   * Helper method for dynamic loading of objects
   */
  void dynamicRegistration(const Parameters & params);

  /**
   * Recursively loads libraries and dependencies in the proper order to fully register a
   * MOOSE application that may have several dependencies. REQUIRES: dynamic linking loader support.
   */
  //void loadLibraryAndDependencies(const std::string & library_filename, const std::string & registration_method_name, Factory * factory);
  void loadLibraryAndDependencies(const std::string & library_filename, const Parameters & params);

  /// Constructor is protected so that this object is constructed through the AppFactory object
  MooseApp(InputParameters parameters);
  MooseApp(const std::string & name, InputParameters parameters);

  /// Don't run the simulation, just complete all of the mesh preperation steps and exit
  virtual void meshOnly(std::string mesh_file_name);

  /**
   * NOTE: This is an internal function meant for MOOSE use only!
   *
   * Register a piece of recoverable data.  This is data that will get
   * written / read to / from a restart file.
   *
   * However, this data will ONLY get read from the restart file during a RECOVERY operation!
   *
   * @param name The full (unique) name.
   */
  virtual void registerRecoverableData(std::string name);

  /// The name of this object
  std::string _name;

  /// Parameters of this object
  InputParameters _pars;

  /// The string representation of the type of this object as registered (see registerApp(AppName))
  const std::string _type;

  /// The MPI communicator this App is going to use
  const MooseSharedPointer<Parallel::Communicator> _comm;

  /// Input file name used
  std::string _input_filename;

  /// The output file basename
  std::string _output_file_base;

  /// Whether or not an output position has been set for this app
  bool _output_position_set;

  /// The output position
  Point _output_position;

  /// Whether or not an start time has been set
  bool _start_time_set;

  /// The time at which to start the simulation
  Real _start_time;

  /// Offset of the local App time to the "global" problem time
  Real _global_time_offset;

  /// Command line object
  MooseSharedPointer<CommandLine> _command_line;

  /// Syntax of the input file
  Syntax _syntax;

  /// OutputWarehouse object for this App
  OutputWarehouse _output_warehouse;

  /// Input parameter storage structure (this is a raw pointer so the destruction time can be explicitly controlled)
  InputParameterWarehouse * _input_parameter_warehouse;

  /// The Factory responsible for building Actions
  ActionFactory _action_factory;

  /// Where built actions are stored
  ActionWarehouse _action_warehouse;

  /// Parser for parsing the input file
  Parser _parser;

  /// Pointer to the executioner of this run (typically build by actions)
  MooseSharedPointer<Executioner> _executioner;

  /// Boolean to indicate whether to use a Nonlinear or EigenSystem (inspected by actions)
  bool _use_nonlinear;

  /// System Information
  MooseSharedPointer<SystemInfo> _sys_info;

  /// Indicates whether warnings, errors, or no output is displayed when unused parameters are detected
  enum UNUSED_CHECK { OFF, WARN_UNUSED, ERROR_UNUSED } _enable_unused_check;

  Factory _factory;


  /// Indicates whether warnings or errors are displayed when overridden parameters are detected
  bool _error_overridden;
  bool _ready_to_exit;

  /// This variable indicates when a request has been made to restart from an Exodus file
  bool _initial_from_file;

  /// This variable indicates that ParallelMesh should be used for the libMesh mesh underlying MooseMesh.
  bool _parallel_mesh_on_command_line;

  /// Whether or not this is a recovery run
  bool _recover;

  /// Whether or not this is a restart run
  bool _restart;

  /// The base name to recover from.  If blank then we will find the newest recovery file.
  std::string _recover_base;

  /// Whether or not this simulation should only run half its transient (useful for testing recovery)
  bool _half_transient;

  /// Map of outputer name and file number (used by MultiApps to propagate file numbers down through the multiapps)
  std::map<std::string, unsigned int> _output_file_numbers;

  /// Legacy Uo Aux computation flag
  bool _legacy_uo_aux_computation_default;

  /// Legacy Uo Initialization flag
  bool _legacy_uo_initialization_default;

  /// True when using legacy constructors
  bool _legacy_constructors;

  /// true if we want to just check the input file
  bool _check_input;

  /// The library, registration method and the handle to the method
  std::map<std::pair<std::string, std::string>, void *> _lib_handles;

private:
  /// Where the restartable data is held (indexed on tid)
  RestartableDatas _restartable_data;

  /// Data names that will only be read from the restart file during RECOVERY
  std::set<std::string> _recoverable_data;

  /// Enumeration for holding the valid types of dynamic registrations allowed
  enum RegistrationType { APPLICATION, OBJECT, SYNTAX };

  ///@{
  /**
   * Sets the restart/recover flags
   * @param state The state to set the flag to
   */
  void setRestart(const bool & value){ _restart = value; }
  void setRecover(const bool & value){ _recover = value; }
  ///@}

  // Allow FEProblem to set the recover/restart state, so make it a friend
  friend class FEProblem;
  friend class Restartable;
  friend class SubProblem;
};

template <typename T>
const T &
MooseApp::getParam(const std::string & name)
{
  return InputParameters::getParamHelper(name, _pars, static_cast<T *>(0));
}

template <typename T>
const T &
MooseApp::getParam(const std::string & name) const
{
  return InputParameters::getParamHelper(name, _pars, static_cast<T *>(0));
}

#endif /* MOOSEAPP_H */
