// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2020.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: George Rosenberger $
// $Authors: George Rosenberger, Chris Bielow $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/CONCEPT/Exception.h>
#include <OpenMS/DATASTRUCTURES/OSWData.h>
#include <OpenMS/DATASTRUCTURES/String.h>
#include <OpenMS/KERNEL/StandardTypes.h>

#include <array>
#include <map>

namespace OpenMS
{
  /**
    @brief This class serves for reading in and writing OpenSWATH OSW files.

    The reader and writer returns data in a format suitable for PercolatorAdapter.
    OSW files have a flexible data structure. They contain all peptide query
    parameters of TraML/PQP files with the detected and quantified features of
    OpenSwathWorkflow (feature, feature_ms1, feature_ms2 & feature_transition).

    The OSWFile reader extracts the feature information from the OSW file for
    each level (MS1, MS2 & transition) separately and generates Percolator input
    files. For each of the three Percolator reports, OSWFile writer adds a table
    (score_ms1, score_ms2, score_transition) with the respective confidence metrics.
    These tables can be mapped to the corresponding feature tables, are very similar
    to PyProphet results and can thus be used interchangeably.
    
  */
  class OPENMS_DLLAPI OSWFile
  {
public:
  
    enum class OSWLevel
    {
      MS1,
      MS2,
      TRANSITION,
      SIZE_OF_OSWLEVEL
    };
    static const std::array<std::string, (Size)OSWLevel::SIZE_OF_OSWLEVEL> names_of_oswlevel;

    struct PercolatorFeature
    {
      PercolatorFeature(double score, double qvalue, double pep)
       : score(score), qvalue(qvalue), posterior_error_prob(pep)
       {}
      PercolatorFeature(const PercolatorFeature& rhs) = default;
      
      double score;
      double qvalue;
      double posterior_error_prob;
    };

    /**
      @brief Reads an OSW SQLite file and returns the data on MS1-, MS2- or transition-level
             as ostream (e.g. stringstream or ofstream).
    */
    static void readToPIN(const std::string& filename, const OSWFile::OSWLevel osw_level, std::ostream& pin_output,
                          const double ipf_max_peakgroup_pep, const double ipf_max_transition_isotope_overlap, const double ipf_min_transition_sn);

    /**
    @brief Updates an OpenSWATH OSW SQLite file with the MS1-, MS2- or transition-level results of Percolator.
    */
    static void writeFromPercolator(const std::string& in_osw, const OSWFile::OSWLevel osw_level, const std::map< std::string, PercolatorFeature >& features);

    /// read data from an SQLLite OSW file @p filename into @p swath_result
    static void read(const String& filename, OSWData& swath_result);

  };

} // namespace OpenMS

