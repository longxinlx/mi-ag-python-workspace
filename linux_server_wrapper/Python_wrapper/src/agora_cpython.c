// gcc -shared -o libagora_rtc_python.so agora_cpython.c  $(python-config
// --cflags --ldflags)  -lagora_rtc_c  -L ./C_language_wrapper/agora_sdk/

// gcc -shared -o libagora_rtc_python.so agora_cpython.c  -lagora_rtc_c  -L
// ./C_language_wrapper/agora_sdk/ -lpython3.6m

#define PY_SSIZE_T_CLEAN
#include <python3.10/Python.h>
#include "../../C_wrapper/src/agora_rtc_wrapper.h"

static PyObject* callback_func = NULL;
static PyObject* arglist = NULL;

static PyObject* audio_frame_observer_func = NULL;
static PyObject* audio_arglist = NULL;


// typedef void (*c_callback)(void *protobuf, int size, int fun_num);
static void c_callback_func(void* protobuf, int size, int fun_num) {
  if (callback_func != NULL) {
    PyGILState_STATE state = PyGILState_Ensure();
    arglist = Py_BuildValue("y#i", protobuf, size, fun_num);
    if (PyCallable_Check(callback_func)) {
      PyObject* pResult = PyObject_CallObject(callback_func, arglist);
      if (pResult == NULL) {
        PyErr_Print();
      }
      Py_XDECREF(pResult);
      Py_XDECREF(arglist);
    }
    PyGILState_Release(state);
  }
}

static void c_audio_frame_observer_func(const char* userId, int samplesPerSec,
                                        int channel_num, const char* buffer10ms,
                                        int bufferSize) {
  if (callback_func != NULL) {
    PyGILState_STATE state = PyGILState_Ensure();
    audio_arglist = Py_BuildValue("y#i", buffer10ms, bufferSize, userId);
    if (PyCallable_Check(callback_func)) {
      PyObject* pResult =
          PyObject_CallObject(audio_frame_observer_func, audio_arglist);
      if (pResult == NULL) {
        PyErr_Print();
      }
      Py_XDECREF(pResult);
      Py_XDECREF(audio_arglist);
    }
    PyGILState_Release(state);
  }
}

static PyObject* init_agora_engine(PyObject* self, PyObject* args) {
  char* config_path;
  int size;

  if (!PyArg_ParseTuple(args, "s#", &config_path, &size)) {
    return NULL;
  }
//   printf("it is %s \n",config_path);
  int result = init(config_path);
  return Py_BuildValue("i", result);
}

static PyObject* destroy_agora_engine(PyObject* self, PyObject* args) {
  int result = destroy();
  return Py_BuildValue("i", result);
}

static PyObject* joinAgoraChannel(PyObject* self, PyObject* args) {
  int result = joinChannel();
  return Py_BuildValue("i", result);
}

static PyObject* leaveAgoraChannel(PyObject* self, PyObject* args) {
  int result = leaveChannel();
  return Py_BuildValue("i", result);
}

static PyObject* send_pcm_audio(PyObject* self, PyObject* args) {
  char* buffer;
  int buffer_size;
  int _size;

  int pts = 0;
  // int buffer_size = 320;

  // if (!PyArg_ParseTuple(args, "y#i",&buffer,&real_size,&buffer_size)) {
  //     return NULL;
  // }
  // printf("real_size %d %d\n",real_size,buffer_size);
  // buffer_size = 320;
  if (!PyArg_ParseTuple(args, "y#ii", &buffer, &buffer_size,&_size,&pts)) {
    return NULL;
  }
  int result = SendPcmAudio(buffer, buffer_size,pts);

  return Py_BuildValue("i", result);
}

static PyObject* send_yuv_video(PyObject* self, PyObject* args) {
  char* buffer;
  int buffer_size;
  int width = 0;
  int height = 0;
  int pts = 0;
  // int buffer_size = 320;

  // if (!PyArg_ParseTuple(args, "y#i",&buffer,&real_size,&buffer_size)) {
  //     return NULL;
  // }
  // printf("real_size %d %d\n",real_size,buffer_size);
  // buffer_size = 320;
  if (!PyArg_ParseTuple(args, "y#iii", &buffer, &buffer_size,&width,&height,&pts)) {
    return NULL;
  }
  // printf("real_size %d %d %d\n",buffer_size,width,height);
  int result = SendVideo(buffer,width,height,pts);

  return Py_BuildValue("i", result);
}

static PyObject* registerAgoraCallback(PyObject* self, PyObject* args) {
  PyObject* temp;

  if (!PyArg_ParseTuple(args, "O:agora_callback_fun", &temp)) {
    return NULL;
  }

  if (!PyCallable_Check(temp)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }
  //   Py_XINCREF(temp);          /* Add a reference to new callback */
  Py_XDECREF(callback_func); /* Dispose of previous callback */
  callback_func = temp;
  registerCallback(c_callback_func);

  return Py_BuildValue("i", 0);
}

static PyObject* registerAgoraAudioFrameHandler(PyObject* self,
                                                PyObject* args) {
  PyObject* temp;

  if (!PyArg_ParseTuple(args, "O:agora_callback_fun", &temp)) {
    return NULL;
  }

  if (!PyCallable_Check(temp)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }
  //   Py_XINCREF(temp);          /* Add a reference to new callback */
  Py_XDECREF(audio_frame_observer_func); /* Dispose of previous callback */
  audio_frame_observer_func = temp;
  registerAudioFrameCallback(c_audio_frame_observer_func);

  return Py_BuildValue("i", 0);
}

static PyMethodDef libagora_rtc_pythonMethods[] = {
    {"init_agora_engine", init_agora_engine, METH_VARARGS, "init_agora_engine"},
    {"destroy_agora_engine", destroy_agora_engine, METH_VARARGS,
     "destroy_agora_engine"},
    {"joinAgoraChannel", joinAgoraChannel, METH_VARARGS, "joinAgoraChannel"},
    {"leaveAgoraChannel", leaveAgoraChannel, METH_VARARGS, "leaveAgoraChannel"},
    {"send_pcm_audio", send_pcm_audio, METH_VARARGS, "send_pcm_audio"},
    {"send_yuv_video", send_yuv_video, METH_VARARGS, "send_yuv_video"},
    {"registerAgoraCallback", registerAgoraCallback, METH_VARARGS,
     "registerAgoraCallback"},
    {"registerAgoraAudioFrameHandler", registerAgoraAudioFrameHandler, METH_VARARGS,
     "registerAgoraAudioFrameHandler"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef libagora_rtc_python_module = {
    PyModuleDef_HEAD_INIT,
    "libagora_rtc_python",
    "libagora_rtc_python module",
    -1,
    libagora_rtc_pythonMethods,
    NULL,  // m_slots should be set as NULL (PyModule_Undef)
    NULL,  // m_gc
    NULL,  // m_traverse
    NULL,  // m_clear
    NULL   // m_free
};

PyMODINIT_FUNC PyInit_libagora_rtc_python(void) {
  return PyModule_Create(&libagora_rtc_python_module);
}
