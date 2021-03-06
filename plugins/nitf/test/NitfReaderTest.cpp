/******************************************************************************
* Copyright (c) 2012, Michael P. Gerlek (mpg@flaxen.com)
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
*     * Neither the name of Hobu, Inc. or Flaxen Consulting LLC nor the
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

#include "gtest/gtest.h"

#include <boost/uuid/uuid_io.hpp>

#include <pdal/PointBuffer.hpp>
#include <pdal/PipelineManager.hpp>
#include <pdal/PipelineReader.hpp>
#include <pdal/StageFactory.hpp>

#include "Support.hpp"

#include <iostream>

#ifdef PDAL_COMPILER_GCC
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <boost/property_tree/xml_parser.hpp>

using namespace pdal;

TEST(NitfReaderTest, test_one)
{
    StageFactory f;

    Options nitf_opts;
    nitf_opts.add("filename", Support::datapath("nitf/autzen-utm10.ntf"));
    nitf_opts.add("count", 750);

    PointContext ctx;
    
    ReaderPtr nitf_reader(f.createReader("readers.nitf"));
    EXPECT_TRUE(nitf_reader.get());
    nitf_reader->setOptions(nitf_opts);
    nitf_reader->prepare(ctx);
    PointBufferSet pbSet = nitf_reader->execute(ctx);
    EXPECT_EQ(nitf_reader->getDescription(), "NITF Reader");
    EXPECT_EQ(pbSet.size(), 1u);
    PointBufferPtr buf = *pbSet.begin();

    // check metadata
//ABELL
/**
    {
        Metadata metadata = nitf_reader.getMetadata();
        /////////////////////////////////////////////////EXPECT_EQ(metadatums.size(), 80u);
        EXPECT_EQ(metadata.toPTree().get<std::string>("metadata.FH_FDT.value"), "20120323002946");
    }
**/

    //
    // read LAS
    //
    Options las_opts;
    las_opts.add("count", 750);
    las_opts.add("filename", Support::datapath("nitf/autzen-utm10.las"));

    PointContext ctx2;

    ReaderPtr las_reader(f.createReader("readers.las"));
    EXPECT_TRUE(las_reader.get());
    las_reader->setOptions(las_opts);
    las_reader->prepare(ctx2);
    PointBufferSet pbSet2 = las_reader->execute(ctx2);
    EXPECT_EQ(pbSet2.size(), 1u);
    PointBufferPtr buf2 = *pbSet.begin();
    //
    //
    // compare the two buffers
    //
    EXPECT_EQ(buf->size(), buf2->size());

    for (PointId i = 0; i < buf2->size(); i++)
    {
        int32_t nitf_x = buf->getFieldAs<int32_t>(Dimension::Id::X, i);
        int32_t nitf_y = buf->getFieldAs<int32_t>(Dimension::Id::Y, i);
        int32_t nitf_z = buf->getFieldAs<int32_t>(Dimension::Id::Z, i);

        int32_t las_x = buf2->getFieldAs<int32_t>(Dimension::Id::X, i);
        int32_t las_y = buf2->getFieldAs<int32_t>(Dimension::Id::Y, i);
        int32_t las_z = buf2->getFieldAs<int32_t>(Dimension::Id::Z, i);

        EXPECT_EQ(nitf_x, las_x);
        EXPECT_EQ(nitf_y, las_y);
        EXPECT_EQ(nitf_z, las_z);
    }
}


TEST(NitfReaderTest, test_chipper)
{
    Option option("filename", Support::configuredpath("nitf/chipper.xml"));
    Options options(option);

    PointContext ctx;

    PipelineManager mgr;
    PipelineReader specReader(mgr);
    specReader.readPipeline(Support::configuredpath("nitf/chipper.xml"));
    //ABELL - need faux writer or something.
    /**
    mgr.execute();
    StageSequentialIterator* iter = reader.createSequentialIterator(data);
    const uint32_t num_read = iter->read(data);
    EXPECT_EQ(num_read, 13u);

    uint32_t num_blocks = chipper->GetBlockCount();
    EXPECT_EQ(num_blocks, 8u);
    **/
}

TEST(NitfReaderTest, optionSrs)
{
    StageFactory f;

    Options nitfOpts;
    nitfOpts.add("filename", Support::datapath("nitf/autzen-utm10.ntf"));

    std::string sr = "PROJCS[\"NAD83 / UTM zone 11N\",GEOGCS[\"NAD83\",DATUM[\"North_American_Datum_1983\",SPHEROID[\"GRS 1980\",6378137,298.257222101,AUTHORITY[\"EPSG\",\"7019\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6269\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4269\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-123],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AXIS[\"Easting\",EAST],AXIS[\"Northing\",NORTH],AUTHORITY[\"EPSG\",\"26910\"]]";

    nitfOpts.add("spatialreference", sr);

    PointContext ctx;

    ReaderPtr nitfReader(f.createReader("readers.nitf"));
    EXPECT_TRUE(nitfReader.get());
    nitfReader->setOptions(nitfOpts);

    Options lasOpts;
    lasOpts.add("filename", "/dev/null");

    WriterPtr lasWriter(f.createWriter("writers.las"));
    EXPECT_TRUE(lasWriter.get());
    lasWriter->setInput(nitfReader.get());
    lasWriter->setOptions(lasOpts);;

    lasWriter->prepare(ctx);
    PointBufferSet pbSet = lasWriter->execute(ctx);

    EXPECT_EQ(sr, nitfReader->getSpatialReference().getWKT());
    EXPECT_EQ("", lasWriter->getSpatialReference().getWKT());
    EXPECT_EQ(sr, ctx.spatialRef().getWKT());
}
