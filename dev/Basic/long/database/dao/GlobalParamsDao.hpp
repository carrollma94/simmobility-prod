//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/* 
 * File:   GlobalParamsDao.hpp
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 *
 * Created on July 1, 2013, 3:59 PM
 */
#pragma once

#include "database/dao/AbstractDao.hpp"
#include "database/entity/GlobalParams.hpp"


namespace sim_mob {
    namespace long_term {

        DAO_DECLARE_CALLBACKS(GlobalParams);

        /**
         * Data Access Object to GlobalParams table on datasource.
         */
        class GlobalParamsDao : public db::AbstractDao<GlobalParams> {
        public:
            GlobalParamsDao(db::DBConnection* connection);
            virtual ~GlobalParamsDao();

        private:
            /**
             * Fills the given outObj with all values contained on Row. 
             * @param result row with data to fill the out object.
             * @param outObj to fill.
             */
            void FromRow(db::Row& result, GlobalParams& outObj);

            /**
             * Fills the outParam with all values to insert or update on datasource.
             * @param data to get values.
             * @param outParams to put the data parameters.
             * @param update tells if operation is an Update or Insert.
             */
            void ToRow(GlobalParams& data, db::Parameters& outParams, bool update);
        };
    }
}
