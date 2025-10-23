#ifndef Z3DGPUINFO_H
#define Z3DGPUINFO_H

#include <QString>
#include <QStringList>

class QOpenGLContext;
class QOpenGLContextGroup;

#define Z3DGpuInfoInstance Z3DGpuInfo::getInstance()

// This class provides information about the GPU.
// Call probeCurrentIfNeeded() once after a real GL context becomes current
// (e.g., in Z3DCanvas::initializeGL). Capabilities are cached per share-group.
class Z3DGpuInfo
{
public:
  static Z3DGpuInfo& getInstance();
  enum GpuVendor {
    GPU_VENDOR_NVIDIA,
    GPU_VENDOR_ATI,
    GPU_VENDOR_INTEL,
    GPU_VENDOR_UNKNOWN
  };
  Z3DGpuInfo();
  ~Z3DGpuInfo() {}
  // Probe capabilities from the *current* context if not yet done for its share group.
  void probeCurrentIfNeeded();
  void probeCurrentForce() { m_probed = false; probeCurrentIfNeeded(); }
  bool isReady() const { return m_probed; }
  bool isSupported() const { return m_isSupported; }
  QString getNotSupportedReason() const { return m_notSupportedReason; }
  int getGlMajorVersion() const { return m_glMajorVersion; }
  int getGlMinorVersion() const { return m_glMinorVersion; }
  int getGlReleaseVersion() const { return m_glReleaseVersion; }
  int getGlslMajorVersion() const;
  int getGlslMinorVersion() const;
  int getGlslReleaseVersion() const;
  GpuVendor getGpuVendor() const;
  bool isExtensionSupported(QString extension) const;
  QString getGlVersionString() const;
  QString getGlVendorString() const;
  QString getGlRendererString() const;
  QString getGlExtensionsString() const;
  QString getGlShadingLanguageVersionString() const;
  // Profile / VAO (cached from the last probe)
  bool isCoreProfile() const { return m_coreProfile; }
  bool isVAOSupported() const { return m_vaoSupported; }
  // Returns the maximal side length of 1D/2D/3D textures.
  int getMaxTextureSize() const { return m_maxTexureSize; }
#ifdef __APPLE__
  int getMax3DTextureSize() const { return std::min(2048, m_max3DTextureSize); }
#else
  int getMax3DTextureSize() const { return m_max3DTextureSize; }
#endif
  int getMaxTextureImageUnits() const { return m_maxTextureImageUnits; }
  int getMaxVertexTextureImageUnits() const { return m_maxVertexTextureImageUnits; }
  int getMaxGeometryTextureImageUnits() const { return m_maxGeometryTextureImageUnits; }
  int getMaxCombinedTextureImageUnits() const { return m_maxCombinedTextureImageUnits; }
  int getMaxTextureCoordinates() const { return m_maxTextureCoords; }
  int getAvailableTextureMemory() const;
  int getTotalTextureMemory() const;
  bool isFrameBufferObjectSupported() const;
  bool isNonPowerOfTwoTextureSupported() const;
  bool isGeometryShaderSupported() const;
  bool isTextureFilterAnisotropicSupported() const;
  bool isTextureRectangleSupported() const;
  bool isImagingSupported() const;
  bool isColorBufferFloatSupported() const;
  bool isDepthBufferFloatSupported() const;
  bool isTextureFloatSupported() const;
  bool isTextureRGSupported() const;
  float getMaxTextureAnisotropy() const { return m_maxTextureAnisotropy; }
  int getMaxColorAttachments() const { return m_maxColorAttachments; }
  int getMaxDrawBuffer() const { return m_maxDrawBuffer; }
  float getMinSmoothPointSize() const { return m_minSmoothPointSize; }
  float getMaxSmoothPointSize() const { return m_maxSmoothPointSize; }
  float getSmoothPointSizeGranularity() const { return m_smoothPointSizeGranularity; }
  float getMinAliasedPointSize() const { return m_minAliasedPointSize; }
  float getMaxAliasedPointSize() const { return m_maxAliasedPointSize; }
  float getMinSmoothLineWidth() const { return m_minSmoothLineWidth; }
  float getMaxSmoothLineWidth() const { return m_maxSmoothLineWidth; }
  float getSmoothLineWidthGranularity() const { return m_smoothLineWidthGranularity; }
  float getMinAliasedLineWidth() const { return m_minAliasedLineWidth; }
  float getMaxAliasedLineWidth() const { return m_maxAliasedLineWidth; }
  // log useful gpu info
  void logGpuInfo() const;
  QStringList getGpuInfo() const;
  // check availability of special effects
  bool isWeightedAverageSupported() const;
  bool isDualDepthPeelingSupported() const;
  bool isLinkedListSupported() const;
protected:
  // Reads all caps from the *current* context. Requires a current context.
  void detectGpuInfo();
  bool parseVersionString(const QString &versionString, int &major, int &minor, int &release);
private:
  bool m_probed = false;
  // Cached share-group key we last probed for (opaque)
  const void* m_shareKey = nullptr;
  // Cached basic caps
  bool m_coreProfile = false;
  bool m_vaoSupported = false;
  bool m_isSupported = false;         // whether current graphic card is supported
  QString m_notSupportedReason;       // reason if unsupported
  int m_glMajorVersion = 0;
  int m_glMinorVersion = 0;
  int m_glReleaseVersion = 0;
  int m_glslMajorVersion = 0;
  int m_glslMinorVersion = 0;
  int m_glslReleaseVersion = 0;
  QString m_glVersionString;
  QString m_glExtensionsString;
  QString m_glVendorString;
  QString m_glRendererString;
  QString m_glslVersionString;
  GpuVendor m_gpuVendor = GPU_VENDOR_UNKNOWN;
  int   m_maxTexureSize = 0;
  int   m_max3DTextureSize = 0;
  float m_maxTextureAnisotropy = 1.0f;
  int   m_maxColorAttachments = 0;
  int   m_maxDrawBuffer = 0;
  int   m_maxGeometryOutputVertices = -1;
  int m_maxTextureImageUnits = 0;
  int m_maxVertexTextureImageUnits = 0;
  int m_maxGeometryTextureImageUnits = -1;
  int m_maxCombinedTextureImageUnits = 0;
  int m_maxTextureCoords = 0;
  float m_minSmoothPointSize = 0.f;
  float m_maxSmoothPointSize = 0.f;
  float m_smoothPointSizeGranularity = 0.f;
  float m_minAliasedPointSize = 0.f;
  float m_maxAliasedPointSize = 0.f;
  float m_minSmoothLineWidth = 0.f;
  float m_maxSmoothLineWidth = 0.f;
  float m_smoothLineWidthGranularity = 0.f;
  float m_minAliasedLineWidth = 0.f;
  float m_maxAliasedLineWidth = 0.f;
};

#endif // Z3DGPUINFO_H
