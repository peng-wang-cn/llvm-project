//===- CoverageSummaryInfo.cpp - Coverage summary for function/file -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// These structures are used to represent code coverage metrics
// for functions/files.
//
//===----------------------------------------------------------------------===//

#include "CoverageSummaryInfo.h"

using namespace llvm;
using namespace coverage;

static auto sumBranches(const ArrayRef<CountedRegion> &Branches) {
  size_t NumBranches = 0;
  size_t CoveredBranches = 0;
  for (const auto &BR : Branches) {
    if (!BR.TrueFolded) {
      // "True" Condition Branches.
      ++NumBranches;
      if (BR.ExecutionCount > 0)
        ++CoveredBranches;
    }
    if (!BR.FalseFolded) {
      // "False" Condition Branches.
      ++NumBranches;
      if (BR.FalseExecutionCount > 0)
        ++CoveredBranches;
    }
  }
  return BranchCoverageInfo(CoveredBranches, NumBranches);
}

static BranchCoverageInfo
sumBranchExpansions(const CoverageMapping &CM,
                    ArrayRef<ExpansionRecord> Expansions) {
  BranchCoverageInfo BranchCoverage;
  for (const auto &Expansion : Expansions) {
    auto CE = CM.getCoverageForExpansion(Expansion);
    BranchCoverage += sumBranches(CE.getBranches());
    BranchCoverage += sumBranchExpansions(CM, CE.getExpansions());
  }
  return BranchCoverage;
}

// Exclusion-aware helpers
static BranchCoverageInfo sumBranchesExcl(
    const ArrayRef<CountedRegion> &Branches, const LcovExclusionSets *Excl) {
  if (!Excl)
    return sumBranches(Branches);
  size_t NumBranches = 0;
  size_t CoveredBranches = 0;
  for (const auto &BR : Branches) {
    unsigned Line = BR.LineStart;
    if (Excl->LineExcluded.count(Line) || Excl->BranchOnlyExcluded.count(Line))
      continue;
    if (!BR.TrueFolded) {
      ++NumBranches;
      if (BR.ExecutionCount > 0)
        ++CoveredBranches;
    }
    if (!BR.FalseFolded) {
      ++NumBranches;
      if (BR.FalseExecutionCount > 0)
        ++CoveredBranches;
    }
  }
  return BranchCoverageInfo(CoveredBranches, NumBranches);
}

static BranchCoverageInfo sumBranchExpansionsExcl(
    const CoverageMapping &CM, ArrayRef<ExpansionRecord> Expansions,
    const LcovExclusionSets *Excl) {
  if (!Excl)
    return sumBranchExpansions(CM, Expansions);
  BranchCoverageInfo BranchCoverage;
  for (const auto &Expansion : Expansions) {
    auto CE = CM.getCoverageForExpansion(Expansion);
    BranchCoverage += sumBranchesExcl(CE.getBranches(), Excl);
    BranchCoverage += sumBranchExpansionsExcl(CM, CE.getExpansions(), Excl);
  }
  return BranchCoverage;
}

auto sumMCDCPairs(const ArrayRef<MCDCRecord> &Records) {
  size_t NumPairs = 0, CoveredPairs = 0;
  for (const auto &Record : Records) {
    const auto NumConditions = Record.getNumConditions();
    for (unsigned C = 0; C < NumConditions; C++) {
      if (!Record.isCondFolded(C)) {
        ++NumPairs;
        if (Record.isConditionIndependencePairCovered(C))
          ++CoveredPairs;
      }
    }
  }
  return MCDCCoverageInfo(CoveredPairs, NumPairs);
}

static std::pair<RegionCoverageInfo, LineCoverageInfo>
sumRegions(ArrayRef<CountedRegion> CodeRegions, const CoverageData &CD) {
  // Compute the region coverage.
  size_t NumCodeRegions = 0, CoveredRegions = 0;
  for (auto &CR : CodeRegions) {
    if (CR.Kind != CounterMappingRegion::CodeRegion)
      continue;
    ++NumCodeRegions;
    if (CR.ExecutionCount != 0)
      ++CoveredRegions;
  }

  // Compute the line coverage
  size_t NumLines = 0, CoveredLines = 0;
  for (const auto &LCS : getLineCoverageStats(CD)) {
    if (!LCS.isMapped())
      continue;
    ++NumLines;
    if (LCS.getExecutionCount())
      ++CoveredLines;
  }

  return {RegionCoverageInfo(CoveredRegions, NumCodeRegions),
          LineCoverageInfo(CoveredLines, NumLines)};
}

static std::pair<RegionCoverageInfo, LineCoverageInfo>
sumRegionsExcl(ArrayRef<CountedRegion> CodeRegions, const CoverageData &CD,
               const LcovExclusionSets *Excl) {
  if (!Excl)
    return sumRegions(CodeRegions, CD);
  // Regions: exclude regions whose starting line is excluded from lines.
  size_t NumCodeRegions = 0, CoveredRegions = 0;
  for (auto &CR : CodeRegions) {
    if (CR.Kind != CounterMappingRegion::CodeRegion)
      continue;
    if (Excl->LineExcluded.count(CR.LineStart))
      continue;
    ++NumCodeRegions;
    if (CR.ExecutionCount != 0)
      ++CoveredRegions;
  }

  // Lines: exclude line entries listed in LineExcluded.
  size_t NumLines = 0, CoveredLines = 0;
  for (const auto &LCS : getLineCoverageStats(CD)) {
    if (!LCS.isMapped())
      continue;
    if (Excl->LineExcluded.count(LCS.getLine()))
      continue;
    ++NumLines;
    if (LCS.getExecutionCount())
      ++CoveredLines;
  }

  return {RegionCoverageInfo(CoveredRegions, NumCodeRegions),
          LineCoverageInfo(CoveredLines, NumLines)};
}

CoverageDataSummary::CoverageDataSummary(const CoverageData &CD,
                                         ArrayRef<CountedRegion> CodeRegions) {
  std::tie(RegionCoverage, LineCoverage) = sumRegions(CodeRegions, CD);
  BranchCoverage = sumBranches(CD.getBranches());
  MCDCCoverage = sumMCDCPairs(CD.getMCDCRecords());
}

CoverageDataSummary::CoverageDataSummary(const CoverageData &CD,
                                         ArrayRef<CountedRegion> CodeRegions,
                                         const LcovExclusionSets *Excl) {
  std::tie(RegionCoverage, LineCoverage) = sumRegionsExcl(CodeRegions, CD, Excl);
  BranchCoverage = sumBranchesExcl(CD.getBranches(), Excl);
  // MC/DC not affected by LCOV exclusions; treat same as original.
  MCDCCoverage = sumMCDCPairs(CD.getMCDCRecords());
}

FunctionCoverageSummary
FunctionCoverageSummary::get(const CoverageMapping &CM,
                             const coverage::FunctionRecord &Function) {
  CoverageData CD = CM.getCoverageForFunction(Function);

  auto Summary =
      FunctionCoverageSummary(Function.Name, Function.ExecutionCount);

  Summary += CoverageDataSummary(CD, Function.CountedRegions);

  // Compute the branch coverage, including branches from expansions.
  Summary.BranchCoverage += sumBranchExpansions(CM, CD.getExpansions());

  return Summary;
}

// Exclusion-aware overload
FunctionCoverageSummary FunctionCoverageSummary::get(
  const CoverageMapping &CM, const coverage::FunctionRecord &Function,
  const LcovExclusionSets *Excl) {
  CoverageData CD = CM.getCoverageForFunction(Function);

  auto Summary =
    FunctionCoverageSummary(Function.Name, Function.ExecutionCount);

  Summary += CoverageDataSummary(CD, Function.CountedRegions, Excl);

  // Compute the branch coverage, including branches from expansions.
  Summary.BranchCoverage +=
    sumBranchExpansionsExcl(CM, CD.getExpansions(), Excl);

  return Summary;
}

FunctionCoverageSummary
FunctionCoverageSummary::get(const InstantiationGroup &Group,
                             ArrayRef<FunctionCoverageSummary> Summaries) {
  std::string Name;
  if (Group.hasName()) {
    Name = std::string(Group.getName());
  } else {
    llvm::raw_string_ostream OS(Name);
    OS << "Definition at line " << Group.getLine() << ", column "
       << Group.getColumn();
  }

  FunctionCoverageSummary Summary(Name, Group.getTotalExecutionCount());
  Summary.RegionCoverage = Summaries[0].RegionCoverage;
  Summary.LineCoverage = Summaries[0].LineCoverage;
  Summary.BranchCoverage = Summaries[0].BranchCoverage;
  Summary.MCDCCoverage = Summaries[0].MCDCCoverage;
  for (const auto &FCS : Summaries.drop_front()) {
    Summary.RegionCoverage.merge(FCS.RegionCoverage);
    Summary.LineCoverage.merge(FCS.LineCoverage);
    Summary.BranchCoverage.merge(FCS.BranchCoverage);
    Summary.MCDCCoverage.merge(FCS.MCDCCoverage);
  }
  return Summary;
}
