/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#pragma once

#include <memory>
#include <pdal/PointView.hpp>

namespace nanoflann
{
    template<typename Distance, class DatasetAdaptor, int DIM,
        typename IndexType> class KDTreeSingleIndexAdaptor;

    template<class T, class DataSource, typename _DistanceType>
    struct L2_Adaptor;
}

namespace pdal
{

class PDAL_DLL KDIndex
{
public:
    KDIndex(const PointView& buf);
    ~KDIndex();

    std::size_t kdtree_get_point_count() const;

    double kdtree_get_pt(const PointId idx, int dim) const;

    double kdtree_distance(
            const PointId idx_p2,
            point_count_t size) const;

    template <class BBOX>
    bool kdtree_get_bbox(BBOX& bb) const
    {
        if (m_buf.empty())
        {
            bb[0].low = 0.0;
            bb[0].high = 0.0;
            bb[1].low = 0.0;
            bb[1].high = 0.0;
            if (m_3d)
            {
                bb[2].low = 0.0;
                bb[2].high = 0.0;
            }
        }
        else
        {
            BOX3D bounds = m_buf.calculateBounds();

            bb[0].low = bounds.minx;
            bb[0].high = bounds.maxx;
            bb[1].low = bounds.miny;
            bb[1].high = bounds.maxy;
            if (m_3d)
            {
                bb[2].low = bounds.minz;
                bb[2].high = bounds.maxz;
            }
        }
        return true;
    }

    std::vector<std::size_t> radius(
            double const& x,
            double const& y,
            double const& z,
            double const& r) const;

    PointId neighbor(double const& x, double const& y, double const& z) const
    {
        std::vector<PointId> ids = neighbors(x, y, z, 1);
        return ids[0];
    }

    std::vector<PointId> neighbors(
            double const& x,
            double const& y,
            double const& z,
            point_count_t count = 1) const;

    void build();

private:
    const PointView& m_buf;
    bool m_3d;

    typedef nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Adaptor<
        double, KDIndex, double>, KDIndex, -1, std::size_t> my_kd_tree_t;

    std::unique_ptr<my_kd_tree_t> m_index;

    KDIndex(const KDIndex&);
    KDIndex& operator=(KDIndex&);
};

} // namespace pdal

