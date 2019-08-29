#include <QApplication>
#include "mainform.h"
#include <QDebug>



//  ---------------------- Doxygen info ----------------------
//! \file 01_RMLPositionSampleApplication.cpp
//!
//! \brief
//! Test application number 1 for the Reflexxes Motion Libraries
//! (basic position-based interface)
//!
//! \date March 2014
//!
//! \version 1.2.6
//!
//! \author Torsten Kroeger, <info@reflexxes.com> \n
//!
//! \copyright Copyright (C) 2014 Google, Inc.
//! \n
//! \n
//! <b>GNU Lesser General Public License</b>
//! \n
//! \n
//! This file is part of the Type II Reflexxes Motion Library.
//! \n\n
//! The Type II Reflexxes Motion Library is free software: you can redistribute
//! it and/or modify it under the terms of the GNU Lesser General Public License
//! as published by the Free Software Foundation, either version 3 of the
//! License, or (at your option) any later version.
//! \n\n
//! The Type II Reflexxes Motion Library is distributed in the hope that it
//! will be useful, but WITHOUT ANY WARRANTY; without even the implied
//! warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
//! the GNU Lesser General Public License for more details.
//! \n\n
//! You should have received a copy of the GNU Lesser General Public License
//! along with the Type II Reflexxes Motion Library. If not, see
//! <http://www.gnu.org/licenses/>.
//  ----------------------------------------------------------
//   For a convenient reading of this file's source code,
//   please use a tab width of four characters.
//  ----------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>

#include <ReflexxesAPI.h>
#include <RMLPositionFlags.h>
#include <RMLPositionInputParameters.h>
#include <RMLPositionOutputParameters.h>


//*************************************************************************
// defines

#define CYCLE_TIME_IN_SECONDS                   0.001
#define NUMBER_OF_DOFS                          3


//*************************************************************************
// Main function to run the process that contains the test application
//
// This function contains source code to get started with the Type II
// Reflexxes Motion Library. Only a minimum amount of functionality is
// contained in this program: a simple trajectory for a
// three-degree-of-freedom system is executed. This code snippet
// directly corresponds to the example trajectories shown in the
// documentation.
//*************************************************************************
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainForm form;
    form.show();

QVector<QVector3D> vector;
QVector<QVector3D> segment;
vector.clear();

QVector3D vector3d;
float _x;
float _y;
float _z;



    //! [0]


    qDebug() << "exit from w.show();" << endl;
    // ********************************************************************
    // Variable declarations and definitions

    int                         ResultValue = 0;
    ReflexxesAPI                *RML = nullptr;
    RMLPositionInputParameters  *IP  = nullptr;
    RMLPositionOutputParameters *OP  = nullptr;
    RMLPositionFlags            Flags;

    // ********************************************************************
    /// Creating all relevant objects of the Type II Reflexxes Motion Library
    RML = new ReflexxesAPI(NUMBER_OF_DOFS,CYCLE_TIME_IN_SECONDS);
    IP  = new RMLPositionInputParameters(NUMBER_OF_DOFS);
    OP  = new RMLPositionOutputParameters(NUMBER_OF_DOFS);
    /// ********************************************************************
    /// Set-up a timer with a period of one millisecond
    /// (not implemented in this example in order to keep it simple)
    /// ********************************************************************
    printf("-------------------------------------------------------\n"  );
    printf("Reflexxes Motion Libraries                             \n"  );
    printf("Example: 01_RMLPositionSampleApplication               \n\n");
    printf("This example demonstrates the most basic use of the    \n"  );
    printf("Reflexxes API (class ReflexxesAPI) using the position- \n"  );
    printf("based Type II Online Trajectory Generation algorithm.  \n\n");
    printf("Copyright (C) 2014 Google, Inc.                        \n"  );
    printf("-------------------------------------------------------\n"  );
    /// ********************************************************************
    /// Set-up the input parameters
    /// In this test program, arbitrary values are chosen. If executed on a
    /// real robot or mechanical system, the position is read and stored in
    /// an RMLPositionInputParameters::CurrentPositionVector vector object.
    /// For the very first motion after starting the controller, velocities
    /// and acceleration are commonly set to zero. The desired target state
    /// of motion and the motion constraints depend on the robot and the
    /// current task/application.
    /// The internal data structures make use of native C data types
    /// (e.g., IP->CurrentPositionVector->VecData is a pointer to
    /// an array of NUMBER_OF_DOFS double values), such that the Reflexxes
    /// Library can be used in a universal way.

    IP->CurrentPositionVector->VecData      [0] =    0.0      ;
    IP->CurrentPositionVector->VecData      [1] =    0.0      ;
    IP->CurrentPositionVector->VecData      [2] =    0.0      ;

    IP->CurrentVelocityVector->VecData      [0] =    100.0      ;
    IP->CurrentVelocityVector->VecData      [1] =    100.0      ;
    IP->CurrentVelocityVector->VecData      [2] =    100.0      ;

    IP->CurrentAccelerationVector->VecData  [0] =    100.0      ;
    IP->CurrentAccelerationVector->VecData  [1] =    100.0      ;
    IP->CurrentAccelerationVector->VecData  [2] =    100.0      ;

    IP->MaxVelocityVector->VecData          [0] =    100.0      ;
    IP->MaxVelocityVector->VecData          [1] =    100.0      ;
    IP->MaxVelocityVector->VecData          [2] =    100.0      ;

    IP->MaxAccelerationVector->VecData      [0] =    100.0      ;
    IP->MaxAccelerationVector->VecData      [1] =    100.0      ;
    IP->MaxAccelerationVector->VecData      [2] =    100.0      ;

    IP->MaxJerkVector->VecData              [0] =    100.0      ;
    IP->MaxJerkVector->VecData              [1] =    100.0      ;
    IP->MaxJerkVector->VecData              [2] =    100.0      ;

    IP->TargetPositionVector->VecData       [0] =   100.0      ;
    IP->TargetPositionVector->VecData       [1] =   100.0      ;
    IP->TargetPositionVector->VecData       [2] =   100.0      ;

    IP->TargetVelocityVector->VecData       [0] =    50.0       ;
    IP->TargetVelocityVector->VecData       [1] =    50.0       ;
    IP->TargetVelocityVector->VecData       [2] =    50.0       ;

    IP->SelectionVector->VecData            [0] =   true        ;
    IP->SelectionVector->VecData            [1] =   true        ;
    IP->SelectionVector->VecData            [2] =   true        ;
    /// Checking the input parameters (optional)
    if (IP->CheckForValidity())
    {
        printf("Input values are valid!\n");
    }
    else
    {
        printf("Input values are INVALID!\n");
    }
    /// ********************************************************************
    /// Starting the control loop
    while(ResultValue != ReflexxesAPI::RML_FINAL_STATE_REACHED)
    {
        ///****************************************************************
        /// Wait for the next timer tick
        /// (not implemented in this example in order to keep it simple)
        /// ****************************************************************
        _x = IP->CurrentPositionVector->VecData[0];
        _y = IP->CurrentPositionVector->VecData[1];
        _z = IP->CurrentPositionVector->VecData[2];
        vector3d.setX(static_cast<float>(_x));
        vector3d.setY(static_cast<float>(_y));
        vector3d.setZ(static_cast<float>(_z));
        vector.push_back(vector3d);
        /// Calling the Reflexxes OTG algorithm
        ResultValue = RML->RMLPosition(*IP,OP,Flags);
        if(ResultValue < 0)
        {
            printf("An error occurred (%d).\n", ResultValue );
            break;
        }
        else
        {
            printf("Result value = (%d).\n", ResultValue );
        }
        /// ****************************************************************
        /// Here, the new state of motion, that is
        /// - OP->NewPositionVector
        /// - OP->NewVelocityVector
        /// - OP->NewAccelerationVector
        /// can be used as input values for lower level controllers. In the
        /// most simple case, a position controller in actuator space is
        /// used, but the computed state can be applied to many other
        /// controllers (e.g., Cartesian impedance controllers,
        /// operational space controllers).
        /// ****************************************************************

        /// ****************************************************************
        /// Feed the output values of the current control cycle back to
        /// input values of the next control cycle
        *IP->CurrentPositionVector      =   *OP->NewPositionVector      ;
        *IP->CurrentVelocityVector      =   *OP->NewVelocityVector      ;
        *IP->CurrentAccelerationVector  =   *OP->NewAccelerationVector  ;

        //qDebug() << "CurrentPositionVector     - " << *OP->NewPositionVector->VecData;
        //qDebug() << "CurrentVelocityVector     - " << *OP->NewVelocityVector->VecData;
        //qDebug() << "CurrentAccelerationVector - " << *OP->NewAccelerationVector->VecData;
        //qDebug() << endl;
        //OP->Echo();
        IP->Echo();





    }






///    for(int i = 0;i < 10;i++)
///    {
///        vector3d.setX(point);
///        vector3d.setY(point);
///        vector3d.setZ(point);
///        point = point + 0.1F;
///        vector.push_back(vector3d);
///    }



    typedef enum
    {
        SEG_UNDEFINED = 0,
        SEG_LINEAR,
        SEG_ARC,
        SEG_RAPID,
        SEG_DWELL
    }SEGMENT_TYPE;



/// segment // vector 3d
/// vector  // vector 3d

    SEGMENT_TYPE _seg_type(SEG_ARC);
    double _arc_radius_begin;
    double _arc_radius_end;
    double _arc_begin_x;
    double _arc_begin_y;
    double _arc_end_x;
    double _arc_end_y;
    double _arc_center_x;
    double _arc_center_y;
    double _arc_center_i;
    double _arc_center_j;





    _arc_begin_x =  142.0; /// X begin
    _arc_begin_y = -2.628; /// Y begin

    _arc_end_x =  140.0; /// X end
    _arc_end_y = -3.089; /// Y end

    _arc_center_i =  10.905; /// I
    _arc_center_j = -42.374; /// J

    _arc_center_x = _arc_begin_x + _arc_center_i; /// X center position
    _arc_center_y = _arc_begin_y + _arc_center_j; /// Y center position

    _arc_radius_begin = fabs(_arc_center_x - _arc_begin_x);
    _arc_radius_end   = fabs(_arc_center_x - _arc_end_x);

    qDebug() << endl;
    qDebug() << "==========================================";
    qDebug() << "_arc_begin_x = "      << _arc_begin_x;
    qDebug() << "_arc_begin_y = "      << _arc_begin_y;
    qDebug() << "_arc_end_x = "        << _arc_end_x;
    qDebug() << "_arc_end_y = "        << _arc_end_y;
    qDebug() << "_arc_center_i = "     << _arc_center_i;
    qDebug() << "_arc_center_j = "     << _arc_center_j;
    qDebug() << "_arc_center_x = "     << _arc_center_x;
    qDebug() << "_arc_center_y = "     << _arc_center_y;
    qDebug() << "_arc_radius_begin = " << _arc_radius_begin;
    qDebug() << "_arc_radius_end   = " << _arc_radius_end;
    qDebug() << "==========================================";


    QVector2D vector_2d;
    vector_2d.setX(static_cast<float>(_arc_end_x));
    vector_2d.setY(static_cast<float>(_arc_end_y));
    qDebug() << "2D length" << vector_2d.length();


    qDebug() << "2D distance - " << vector_2d.distanceToPoint(QVector2D(10,10));




    switch(_seg_type)
    {
        case SEG_ARC :






            break;
        default :
            break;
    }

//    form.plotAddData(vector);
//    form.plotShow(true);

    // ********************************************************************
    // Deleting the objects of the Reflexxes Motion Library end terminating
    // the process

    delete  RML;
    delete  IP;
    delete  OP;

    return app.exec();
}
