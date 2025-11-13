#ifndef PYTHON_INTERPRETER_H_
#define PYTHON_INTERPRETER_H_

#include <memory>
#include <mutex>
#include <string>
#include <variant>
#include <array>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#include <wchar.h>
#include <conio.h>

#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <pybind11/embed.h>
namespace py = pybind11;

enum PyResponseType
{
  kNone = -1, 
  kNull,
  kNormal,
  kError
};

struct PyResponse
{
  PyResponseType type;
  std::u8string content;

  PyResponse();
  PyResponse(PyResponseType type_, const std::u8string& content_);
  ~PyResponse() = default;
};

class PyInterpreterSingleton
{
  private:
  fs::path py_root_path; 
  PyResponse response_buf;
  std::mutex response_buf_mutex;

  PyInterpreterSingleton();
  PyInterpreterSingleton(const PyInterpreterSingleton&)            = delete;
  PyInterpreterSingleton(PyInterpreterSingleton&&)                 = delete;
  PyInterpreterSingleton& operator=(const PyInterpreterSingleton&) = delete;
  ~PyInterpreterSingleton()                                        = default;

  public:
  static PyInterpreterSingleton& GetInstance();
  void SetProperties(const char* main_argv_0_, const char* err_msg_);

  void StockUpResponse(PyResponseType type_, std::u8string response_);
  PyResponse& GetResponseBuf();
  void ClearResponseBuf();
  void SendCommand(const std::u8string& command_);

  static void WritePyInterpResponseLog(const std::u8string& line_);
  static bool ThereIsPyConfigErr(PyStatus& status_, const char* err_msg_);
};

#endif // PYTHON_INTERPRETER_H_