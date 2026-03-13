//===- CoverageExporter.h - Code coverage exporter ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class defines a code coverage exporter interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_COV_COVERAGEEXPORTER_H
#define LLVM_COV_COVERAGEEXPORTER_H

#include "CoverageFilters.h"
#include "CoverageSummaryInfo.h"
#include "CoverageViewOptions.h"
#include "llvm/ProfileData/Coverage/CoverageMapping.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {

/// Exports the code coverage information.
class CoverageExporter {
protected:
  /// The full CoverageMapping object to export.
  const coverage::CoverageMapping &Coverage;

  /// The options passed to the tool.
  const CoverageViewOptions &Options;

  /// Output stream to print to.
  raw_ostream &OS;

  /// Map from source file path to its exclusion sets.
  /// Only populated when RespectLcovExclusions is enabled.
  StringMap<LcovExclusionSets> FileExclusions;

  CoverageExporter(const coverage::CoverageMapping &CoverageMapping,
                   const CoverageViewOptions &Options, raw_ostream &OS)
      : Coverage(CoverageMapping), Options(Options), OS(OS) {}

  /// Returns exclusion sets for a file, or nullptr if none.
  const LcovExclusionSets *getExclusions(StringRef Filename) const {
    if (!Options.RespectLcovExclusions)
      return nullptr;
    auto It = FileExclusions.find(Filename);
    return It != FileExclusions.end() ? &It->second : nullptr;
  }

public:
  /// Check if a line is excluded from line coverage.
  bool isLineExcluded(StringRef Filename, unsigned LineNo) const {
    const auto *Excl = getExclusions(Filename);
    if (!Excl)
      return false;
    return Excl->LineExcluded.count(LineNo) ||
           Excl->UnreachableExcluded.count(LineNo);
  }

  /// Check if a line is excluded from branch coverage.
  bool isBranchExcluded(StringRef Filename, unsigned LineNo) const {
    const auto *Excl = getExclusions(Filename);
    if (!Excl)
      return false;
    return Excl->LineExcluded.count(LineNo) ||
           Excl->BranchOnlyExcluded.count(LineNo) ||
           Excl->ExceptionBranchOnlyExcluded.count(LineNo) ||
           Excl->UnreachableExcluded.count(LineNo);
  }

  virtual ~CoverageExporter(){};

  /// Render the CoverageMapping object.
  virtual void renderRoot(const CoverageFilters &IgnoreFilters) = 0;

  /// Render the CoverageMapping object for specified source files.
  virtual void renderRoot(ArrayRef<std::string> SourceFiles) = 0;

  /// Set exclusions for a source file.
  void setExclusions(StringRef Filename, LcovExclusionSets Exclusions) {
    FileExclusions.insert({Filename, std::move(Exclusions)});
  }

  /// Get the options reference.
  const CoverageViewOptions &getOptions() const { return Options; }
};

} // end namespace llvm

#endif // LLVM_COV_COVERAGEEXPORTER_H
