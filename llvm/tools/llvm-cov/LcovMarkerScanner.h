//===- LcovMarkerScanner.h - LCOV marker scanning utilities --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// This header provides a utility to scan a source buffer for LCOV exclusion
/// markers and return a populated LcovExclusionSets structure.
/// Recognized markers (per latest LCOV geninfo.1):
///  - LCOV_EXCL_LINE, LCOV_EXCL_START/STOP
///  - LCOV_EXCL_BR_LINE, LCOV_EXCL_BR_START/STOP
///  - LCOV_EXCL_EXCEPTION_BR_LINE, LCOV_EXCL_EXCEPTION_BR_START/STOP
///  - LCOV_UNREACHABLE_LINE, LCOV_UNREACHABLE_START/STOP
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_COV_LCOVMARKERSCANNER_H
#define LLVM_COV_LCOVMARKERSCANNER_H

#include "CoverageSummaryInfo.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {

static inline LcovExclusionSets scanLcovExclusionsFromBuffer(StringRef Text) {
  LcovExclusionSets Sets;
  bool InLineExclRegion = false;
  bool InBrExclRegion = false;
  bool InExBrExclRegion = false;
  bool InUnreachRegion = false;

  unsigned LineNo = 0;
  StringRef Remaining = Text;
  while (!Remaining.empty()) {
    auto Pair = Remaining.split('\n');
    StringRef Line = Pair.first;
    Remaining = Pair.second;
    ++LineNo;

    if (Line.contains("LCOV_EXCL_START"))
      InLineExclRegion = true;
    if (Line.contains("LCOV_EXCL_STOP"))
      InLineExclRegion = false;

    if (Line.contains("LCOV_EXCL_BR_START"))
      InBrExclRegion = true;
    if (Line.contains("LCOV_EXCL_BR_STOP"))
      InBrExclRegion = false;

    if (Line.contains("LCOV_EXCL_EXCEPTION_BR_START"))
      InExBrExclRegion = true;
    if (Line.contains("LCOV_EXCL_EXCEPTION_BR_STOP"))
      InExBrExclRegion = false;

    if (Line.contains("LCOV_UNREACHABLE_START"))
      InUnreachRegion = true;
    if (Line.contains("LCOV_UNREACHABLE_STOP"))
      InUnreachRegion = false;

    if (Line.contains("LCOV_EXCL_LINE"))
      Sets.LineExcluded.insert(LineNo);
    if (Line.contains("LCOV_UNREACHABLE_LINE")) {
      Sets.LineExcluded.insert(LineNo);
      Sets.UnreachableExcluded.insert(LineNo);
    }
    if (Line.contains("LCOV_EXCL_BR_LINE"))
      Sets.BranchOnlyExcluded.insert(LineNo);
    if (Line.contains("LCOV_EXCL_EXCEPTION_BR_LINE"))
      Sets.ExceptionBranchOnlyExcluded.insert(LineNo);

    if (InLineExclRegion)
      Sets.LineExcluded.insert(LineNo);
    if (InBrExclRegion)
      Sets.BranchOnlyExcluded.insert(LineNo);
    if (InExBrExclRegion)
      Sets.ExceptionBranchOnlyExcluded.insert(LineNo);
    if (InUnreachRegion) {
      Sets.LineExcluded.insert(LineNo);
      Sets.UnreachableExcluded.insert(LineNo);
    }
  }

  return Sets;
}

} // end namespace llvm

#endif // LLVM_COV_LCOVMARKERSCANNER_H
