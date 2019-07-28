/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.scanner.rmcode.core;

public class Coder {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected Coder(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Coder obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        coder_WrapperJNI.delete_Coder(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public Coder() {
    this(coder_WrapperJNI.new_Coder__SWIG_0(), true);
  }

  public Coder(int modulesNumber) {
    this(coder_WrapperJNI.new_Coder__SWIG_1(modulesNumber), true);
  }

  public void setModulesNumber(int modulessNumber) {
    coder_WrapperJNI.Coder_setModulesNumber(swigCPtr, this, modulessNumber);
  }

  public int getModulesNumber() {
    return coder_WrapperJNI.Coder_getModulesNumber(swigCPtr, this);
  }

  public String decodeStringFromMat(SWIGTYPE_p_cv__Mat mat) {
    return coder_WrapperJNI.Coder_decodeStringFromMat(swigCPtr, this, SWIGTYPE_p_cv__Mat.getCPtr(mat));
  }

}
