#include "z3dgpuinfo.h"
#include "zglew.h"

#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QSurfaceFormat>
#include <QStringList>
#include <QProcess>
#include "QsLog.h"

Z3DGpuInfo& Z3DGpuInfo::getInstance()
{
  static Z3DGpuInfo gpuInfo;
  return gpuInfo;
}

Z3DGpuInfo::Z3DGpuInfo()
  : m_isSupported(false)
{
  // Do not call detectGpuInfo() here. No current context yet.
  // Call probeCurrentIfNeeded() from initializeGL when a context is current.
}

void Z3DGpuInfo::probeCurrentIfNeeded()
{
  QOpenGLContext* ctx = QOpenGLContext::currentContext();
  if (!ctx) {
    LWARN() << "[Z3DGpuInfo] probe requested without a current context.";
    return;
  }
  // Cache by share-group so all shared contexts reuse the same caps
  const void* key = static_cast<const void*>(ctx->shareGroup());
  // If we've already probed this share-group and haven't been force-reset, bail
  if (m_shareKey == key && m_probed) { return; }
  // Basic caps that depend on the actual context created
  const QSurfaceFormat fmt = ctx->format();
  m_coreProfile  = (fmt.profile() == QSurfaceFormat::CoreProfile);
  m_vaoSupported =
      (GLEW_VERSION_3_0 == GL_TRUE) ||
      (GLEW_ARB_vertex_array_object == GL_TRUE) ||
      isExtensionSupported("GL_APPLE_vertex_array_object");
  // Read full GL capabilities (requires current context)
  detectGpuInfo();
  // Probe complete for this share-group
  m_shareKey = key;
  m_probed = true;
  LINFO() << "[Z3DGpuInfo] Probed GL"
          << m_glMajorVersion << "." << m_glMinorVersion
          << (m_coreProfile ? " Core" : " Compat")
          << " VAO:" << (m_vaoSupported ? "yes" : "no");
}

int Z3DGpuInfo::getGlslMajorVersion() const
{
  if (isSupported()) {
    return m_glslMajorVersion;
  } else {
    LERROR() << "Current GPU card not supported. This function call should not happen.";
    return -1;
  }
}

int Z3DGpuInfo::getGlslMinorVersion() const
{
  if (isSupported()) {
    return m_glslMinorVersion;
  } else {
    LERROR() << "Current GPU card not supported. This function call should not happen.";
    return -1;
  }
}

int Z3DGpuInfo::getGlslReleaseVersion() const
{
  if (isSupported()) {
    return m_glslReleaseVersion;
  } else {
    LERROR() << "Current GPU card not supported. This function call should not happen.";
    return -1;
  }
}

Z3DGpuInfo::GpuVendor Z3DGpuInfo::getGpuVendor() const
{
  return m_gpuVendor;
}

bool Z3DGpuInfo::isExtensionSupported(QString extension) const
{
  return m_glExtensionsString.contains(extension, Qt::CaseInsensitive);
}

QString Z3DGpuInfo::getGlVersionString() const { return m_glVersionString; }
QString Z3DGpuInfo::getGlVendorString()  const { return m_glVendorString; }
QString Z3DGpuInfo::getGlRendererString()const { return m_glRendererString; }
QString Z3DGpuInfo::getGlShadingLanguageVersionString() const { return m_glslVersionString; }
QString Z3DGpuInfo::getGlExtensionsString() const { return m_glExtensionsString; }

int Z3DGpuInfo::getAvailableTextureMemory() const
{
  int availableTexMem = -1;

  if (m_gpuVendor == GPU_VENDOR_NVIDIA) {
    if(isExtensionSupported("GL_NVX_gpu_memory_info")) {
      int retVal;
      glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &retVal);
      availableTexMem = retVal;
    }
  } else if (m_gpuVendor == GPU_VENDOR_ATI) {
    if(isExtensionSupported("GL_ATI_meminfo")) {
      int retVal[4];
      glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, retVal);
      availableTexMem = retVal[1];
    }
  }

  return availableTexMem;
}

int Z3DGpuInfo::getTotalTextureMemory() const
{
  int totalTexMem = -1;

  if (m_gpuVendor == GPU_VENDOR_NVIDIA) {
    if(isExtensionSupported("GL_NVX_gpu_memory_info")) {
      int retVal;
      glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &retVal);
      totalTexMem = retVal;
    }
  } else if (m_gpuVendor == GPU_VENDOR_ATI) {
    if(isExtensionSupported("GL_ATI_meminfo")) {
      int retVal[4];
      glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, retVal);
      totalTexMem = retVal[0];
    }
  }

  return totalTexMem;
}

bool Z3DGpuInfo::isFrameBufferObjectSupported() const
{ return GLEW_VERSION_3_0 || isExtensionSupported("GL_EXT_framebuffer_object"); }

bool Z3DGpuInfo::isNonPowerOfTwoTextureSupported() const
{ return GLEW_VERSION_2_0 || isExtensionSupported("GL_ARB_texture_non_power_of_two"); }

bool Z3DGpuInfo::isGeometryShaderSupported() const
{
  return GLEW_VERSION_3_2 ||
         isExtensionSupported("GL_ARB_geometry_shader4") ||
         isExtensionSupported("GL_EXT_geometry_shader4");
}

bool Z3DGpuInfo::isTextureFilterAnisotropicSupported() const
{ return isExtensionSupported("GL_EXT_texture_filter_anisotropic"); }

bool Z3DGpuInfo::isTextureRectangleSupported() const
{ return GLEW_VERSION_3_1 || isExtensionSupported("GL_ARB_texture_rectangle"); }

bool Z3DGpuInfo::isImagingSupported() const
{ return isExtensionSupported("GL_ARB_imaging"); }

bool Z3DGpuInfo::isColorBufferFloatSupported() const
{ return GLEW_VERSION_3_0 || isExtensionSupported("GL_ARB_color_buffer_float"); }

bool Z3DGpuInfo::isDepthBufferFloatSupported() const
{ return GLEW_VERSION_3_0 || isExtensionSupported("GL_ARB_depth_buffer_float"); }

bool Z3DGpuInfo::isTextureFloatSupported() const
{ return GLEW_VERSION_3_0 || isExtensionSupported("GL_ARB_texture_float"); }

bool Z3DGpuInfo::isTextureRGSupported() const
{ return GLEW_VERSION_3_0 || isExtensionSupported("GL_ARB_texture_rg"); }

QStringList Z3DGpuInfo::getGpuInfo() const
{
  QStringList info;
  if (!isSupported()) {
    info << QString("Current GPU card is not supported. Reason: %1").arg(m_notSupportedReason);
    info << "3D functions will be disabled.";
    return info;
  }

#ifdef __APPLE__
  QProcess dispInfo;
  dispInfo.start("system_profiler", QStringList() << "SPDisplaysDataType");

  if (dispInfo.waitForFinished(-1))
    info << dispInfo.readAllStandardOutput();
  else
    info << dispInfo.readAllStandardError();
#endif

  info << QString("OpenGL Vendor: %1").arg(m_glVendorString);
  info << QString("OpenGL Renderer: %1").arg(m_glRendererString);
  info << QString("OpenGL Version: %1").arg(m_glVersionString);
  info << QString("OpenGL SL Version: %1").arg(m_glslVersionString);
  info << QString("Profile: %1").arg(m_coreProfile ? "Core" : "Compatibility");
  info << QString("VAO Supported: %1").arg(m_vaoSupported ? "Yes" : "No");
  info << QString("Max Texture Size:              %1").arg(m_maxTexureSize);
  info << QString("Max 3D Texture Size:           %1").arg(m_max3DTextureSize);
  info << QString("Max Color Attachments:         %1").arg(m_maxColorAttachments);
  info << QString("Max Draw Buffer:               %1").arg(m_maxDrawBuffer);
  if(isGeometryShaderSupported() && m_maxGeometryOutputVertices > 0) {
    info << QString("Max GS Output Vertices:        %1").arg(m_maxGeometryOutputVertices);
  }
  info << QString("Max VS Texture Image Units:    %1").arg(m_maxVertexTextureImageUnits);
  if (isGeometryShaderSupported() && m_maxGeometryTextureImageUnits > 0) {
    info << QString("Max GS Texture Image Units:    %1").arg(m_maxGeometryTextureImageUnits);
  }
  info << QString("Max FS Texture Image Units:    %1").arg(m_maxTextureImageUnits);
  info << QString("VS+GS+FS Texture Image Units:  %1").arg(m_maxCombinedTextureImageUnits);
  info << QString("Max Texture Coordinates:       %1").arg(m_maxTextureCoords);

  if(getTotalTextureMemory() != -1) {
    info << QString("Total Graphics Memory Size:    %1 MB").arg(getTotalTextureMemory()/1024);
  }

  if(getAvailableTextureMemory() != -1) {
    info << QString("Available Graphics Memory Size: %1 MB").arg(getAvailableTextureMemory()/1024);
  }

  info << QString("Smooth Point Size Range:       (%1, %2)").arg(m_minSmoothPointSize).arg(m_maxSmoothPointSize);
  info << QString("Smooth Point Size Granularity: %1").arg(m_smoothPointSizeGranularity);
  info << QString("Aliased Point Size Range:      (%1, %2)").arg(m_minAliasedPointSize).arg(m_maxAliasedPointSize);

  info << QString("Smooth Line Width Range:       (%1, %2)").arg(m_minSmoothLineWidth).arg(m_maxSmoothLineWidth);
  info << QString("Smooth Line Width Granularity: %1").arg(m_smoothLineWidthGranularity);
  info << QString("Aliased Line Width Range:      (%1, %2)").arg(m_minAliasedLineWidth).arg(m_maxAliasedLineWidth);

  return info;
}

void Z3DGpuInfo::logGpuInfo() const
{
  if (!isSupported()) {
    LINFO() << "Current GPU card is not supported. Reason: " << m_notSupportedReason;
    LWARN() << "3D functions will be disabled.";
    return;
  }

  LINFO() << "OpenGL Vendor:" << m_glVendorString;
  LINFO() << "OpenGL Renderer:" << m_glRendererString;
  LINFO() << "OpenGL Version:" << m_glVersionString;
  LINFO() << "OpenGL SL Version:" << m_glslVersionString;
  LINFO() << "Profile:" << (m_coreProfile ? "Core" : "Compatibility");
  LINFO() << "VAO Supported:" << (m_vaoSupported ? "Yes" : "No");
  LINFO() << "Max Texture Size:" << m_maxTexureSize;
  LINFO() << "Max 3D Texture Size:" << m_max3DTextureSize;
  LINFO() << "Max Color Attachments:" << m_maxColorAttachments;
  LINFO() << "Max Draw Buffer:" << m_maxDrawBuffer;
  if(isGeometryShaderSupported() && m_maxGeometryOutputVertices > 0) {
    LINFO() << "Max GS Output Vertices:" << m_maxGeometryOutputVertices;
  }
  LINFO() << "Max VS Texture Image Units:" << m_maxVertexTextureImageUnits;
  if (isGeometryShaderSupported() && m_maxGeometryTextureImageUnits > 0) {
    LINFO() << "Max GS Texture Image Units:" << m_maxGeometryTextureImageUnits;
  }
  LINFO() << "Max FS Texture Image Units:" << m_maxTextureImageUnits;
  LINFO() << "VS+GS+FS Texture Image Units:" << m_maxCombinedTextureImageUnits;
  LINFO() << "Max Texture Coordinates:" << m_maxTextureCoords;

  if(getTotalTextureMemory() != -1) {
    LINFO() << "Total Graphics Memory Size:" << getTotalTextureMemory()/1024 << "MB";
  }

  if(getAvailableTextureMemory() != -1) {
    LINFO() << "Available Graphics Memory Size:" << getAvailableTextureMemory()/1024 << "MB";
  }

  LINFO() << "Smooth Point Size Range: (" << m_minSmoothPointSize << "," << m_maxSmoothPointSize << ")";
  LINFO() << "Smooth Point Size Granularity:" << m_smoothPointSizeGranularity;
  LINFO() << "Aliased Point Size Range: (" << m_minAliasedPointSize << "," << m_maxAliasedPointSize << ")";

  LINFO() << "";
}

bool Z3DGpuInfo::isWeightedAverageSupported() const
{
#ifdef _ENABLE_WAVG_
  return Z3DGpuInfoInstance.isTextureRGSupported() && Z3DGpuInfoInstance.isTextureRectangleSupported() &&
         Z3DGpuInfoInstance.isTextureFloatSupported() && Z3DGpuInfoInstance.isImagingSupported() &&
         Z3DGpuInfoInstance.isColorBufferFloatSupported() &&
         Z3DGpuInfoInstance.getMaxColorAttachments() >= 2;
#else
  return false;
#endif
}

bool Z3DGpuInfo::isDualDepthPeelingSupported() const
{
#ifdef _ENABLE_DDP_
  return Z3DGpuInfoInstance.isTextureRGSupported() && Z3DGpuInfoInstance.isTextureRectangleSupported() &&
         Z3DGpuInfoInstance.isTextureFloatSupported() && Z3DGpuInfoInstance.isImagingSupported() &&
         Z3DGpuInfoInstance.isColorBufferFloatSupported() &&
         Z3DGpuInfoInstance.getMaxColorAttachments() >= 8;
#else
  return false;
#endif
}

bool Z3DGpuInfo::isLinkedListSupported() const
{
  return GLEW_VERSION_4_2;
}

void Z3DGpuInfo::detectGpuInfo()
{
  // Requires a current context.
  const GLubyte* vs = glGetString(GL_VERSION);
  const GLubyte* ven = glGetString(GL_VENDOR);
  const GLubyte* ren = glGetString(GL_RENDERER);
  const GLubyte* exts = glGetString(GL_EXTENSIONS);

  m_glVersionString    = vs   ? QString(reinterpret_cast<const char*>(vs))   : QString();
  m_glVendorString     = ven  ? QString(reinterpret_cast<const char*>(ven))  : QString();
  m_glRendererString   = ren  ? QString(reinterpret_cast<const char*>(ren))  : QString();
  m_glExtensionsString = exts ? QString(reinterpret_cast<const char*>(exts)) : QString();

  if (GLEW_VERSION_2_1) {
    if (!isFrameBufferObjectSupported()) {
      m_isSupported = false;
      m_notSupportedReason = "Frame Buffer Object (FBO) is not supported by current OpenGL context.";
      return;
    } else if (!isNonPowerOfTwoTextureSupported()) {
      m_isSupported = false;
      m_notSupportedReason = "Non power of two texture is not supported by current OpenGL context.";
      return;
    } else if (getGpuVendor() == GPU_VENDOR_ATI && isNonPowerOfTwoTextureSupported() &&
               (m_glRendererString.contains("RADEON X", Qt::CaseInsensitive)||
                m_glRendererString.contains("RADEON 9", Qt::CaseInsensitive))) {
      m_isSupported = false;
      m_notSupportedReason = "R300/R400-based cards (Radeon 9500+/X500+) cannot use generic NPOT with mipmaps.";
      return;
    } else if (getGpuVendor() == GPU_VENDOR_NVIDIA && isNonPowerOfTwoTextureSupported() &&
               m_glRendererString.contains("GeForce FX", Qt::CaseInsensitive)) {
      m_isSupported = false;
      m_notSupportedReason = "NV30 (GeForce FX) cannot use NPOT at all despite reporting GL 2.0.";
      return;
    } else {
      m_isSupported = true;
    }

    const char* glslVS = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    m_glslVersionString = glslVS ? QString(glslVS) : QString();

    if (!parseVersionString(m_glVersionString, m_glMajorVersion, m_glMinorVersion, m_glReleaseVersion)) {
      LERROR() << "Malformed OpenGL version string:" << m_glVersionString;
    }

    // GPU Vendor
    if (m_glVendorString.contains("NVIDIA", Qt::CaseInsensitive))
      m_gpuVendor = GPU_VENDOR_NVIDIA;
    else if (m_glVendorString.contains("ATI", Qt::CaseInsensitive) ||
             m_glVendorString.contains("AMD", Qt::CaseInsensitive))
      m_gpuVendor = GPU_VENDOR_ATI;
    else if (m_glVendorString.contains("INTEL", Qt::CaseInsensitive))
      m_gpuVendor = GPU_VENDOR_INTEL;
    else
      m_gpuVendor = GPU_VENDOR_UNKNOWN;

    // GLSL
    if (!parseVersionString(m_glslVersionString, m_glslMajorVersion, m_glslMinorVersion, m_glslReleaseVersion)) {
      LERROR() << "Malformed GLSL version string:" << m_glslVersionString;
    }

    m_maxGeometryOutputVertices = -1;
    if (isGeometryShaderSupported())
      glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &m_maxGeometryOutputVertices);

    // Texturing
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTexureSize);
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &m_max3DTextureSize);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureImageUnits);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &m_maxVertexTextureImageUnits);
    m_maxGeometryTextureImageUnits = -1;
    if (isGeometryShaderSupported())
      glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &m_maxGeometryTextureImageUnits);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxCombinedTextureImageUnits);
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &m_maxTextureCoords);

    if (isTextureFilterAnisotropicSupported())
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_maxTextureAnisotropy);
    else
      m_maxTextureAnisotropy = 1.0f;

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &m_maxColorAttachments);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &m_maxDrawBuffer);

    // Points
    GLfloat range[2];
    glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, range);
    glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &m_smoothPointSizeGranularity);
    m_minSmoothPointSize = range[0];
    m_maxSmoothPointSize = range[1];
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, range);
    m_minAliasedPointSize = range[0];
    m_maxAliasedPointSize = range[1];

    // Lines
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, range);
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &m_smoothLineWidthGranularity);
    m_minSmoothLineWidth = range[0];
    m_maxSmoothLineWidth = range[1];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, range);
    m_minAliasedLineWidth = range[0];
    m_maxAliasedLineWidth = range[1];

  } else {
    m_isSupported = false;
    m_notSupportedReason =
        "Minimum OpenGL version required is 2.1, while current OpenGL version is: \"" +
        m_glVersionString + "\"";
  }
}

// format "2.1[.1] otherstring"
bool Z3DGpuInfo::parseVersionString(const QString &versionString, int &major, int &minor, int &release)
{
  major = -1; minor = -1; release = -1;
  if (versionString.isEmpty()) return false;

  QString str = versionString.mid(0, versionString.indexOf(" "));
  QStringList list = str.split(".");
  if (list.size() < 2 || list.size() > 3) return false;

  bool ok;
  major = list[0].toInt(&ok); if (!ok) { major = -1; return false; }
  minor = list[1].toInt(&ok); if (!ok) { major = -1; minor = -1; return false; }

  if (list.size() > 2) {
    release = list[2].toInt(&ok);
    if (!ok) { major = -1; minor = -1; release = -1; return false; }
  } else {
    release = 0;
  }

  return true;
}
