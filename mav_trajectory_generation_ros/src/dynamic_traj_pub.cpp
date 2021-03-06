#include <mav_trajectory_generation/polynomial_optimization_linear.h>
#include <mav_trajectory_generation/trajectory.h>
#include <mav_trajectory_generation/trajectory_sampling.h>
#include <mav_trajectory_generation_ros/ros_visualization.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Pose.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseStamped.h"
#include "ros/ros.h"


geometry_msgs::PoseArray way_;
void waycb(const geometry_msgs::PoseArray::ConstPtr &msg)
{
    way_ = *msg;
}

float x,y,odom_detected_flag = 0;
/*
void odomcb(const nav_msgs::Odometry::ConstPtr &msg)
{
    x = (msg->pose.pose.position.x);
    y = (msg->pose.pose.position.y);

    odom_detected_flag = 1;
}

float object_y,object_x,pick_goal_x,pick_goal_y;
int aruco_detected_flag = 0;
void arucocb(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    object_y = -(msg->pose.position.x);
    object_x = -(msg->pose.position.y);

    pick_goal_x = x + object_x;
    pick_goal_y = y + object_y;

    aruco_detected_flag = 1;
}
*/
int main(int argc, char **argv)
{
    ros::init(argc, argv, "test_node");
    ros::NodeHandle n;
    //ros::Subscriber odo_sub = n.subscribe<geometry_msgs::PoseArray>("/waypoints", 10, waycb);
    //ros::Subscriber aruco_sub = n.subscribe<geometry_msgs::PoseStamped>("/aruco_single/pose", 10, arucocb);
    ros::Publisher traj_points_pub = n.advertise<geometry_msgs::PoseArray>("points_traj", 10);
    // rate
    ros::Rate sleep_rate(1);
    // int marker_size = markers.markers.size();
    // int trajectory_size = markers.markers[marker_size-1].points.size();
    // for (int i=0 ; i < trajectory_size ; i++)
    // std::cout<<markers.markers[marker_size-1].points[i]<<std::endl;

    //Declaration
    geometry_msgs::PoseArray points;
    geometry_msgs::Pose point;

    while (ros::ok())
    {   

        float x_1,y_1,z_1;
        n.getParam("/dynamic_traj_pub/x_1", x_1);
        n.getParam("/dynamic_traj_pub/y_1", y_1);
        n.getParam("/dynamic_traj_pub/z_1", z_1);

        float x_2,y_2,z_2;
        n.getParam("/dynamic_traj_pub/x_2", x_2);
        n.getParam("/dynamic_traj_pub/y_2", y_2);
        n.getParam("/dynamic_traj_pub/z_2", z_2);

        float x_3,y_3,z_3;
        n.getParam("/dynamic_traj_pub/x_3", x_3);
        n.getParam("/dynamic_traj_pub/y_3", y_3);
        n.getParam("/dynamic_traj_pub/z_3", z_3);

        float x_4,y_4,z_4;
        n.getParam("/dynamic_traj_pub/x_4", x_4);
        n.getParam("/dynamic_traj_pub/y_4", y_4);
        n.getParam("/dynamic_traj_pub/z_4", z_4);

        float x_5,y_5,z_5;
        n.getParam("/dynamic_traj_pub/x_5", x_5);
        n.getParam("/dynamic_traj_pub/y_5", y_5);
        n.getParam("/dynamic_traj_pub/z_5", z_5);

        mav_trajectory_generation::Vertex::Vector vertices;
        const int dimension = 3;
        const int derivative_to_optimize = mav_trajectory_generation::derivative_order::ACCELERATION;
        mav_trajectory_generation::Vertex start(dimension), middle(dimension), end(dimension);

        //Time count
        // ros::Time t0 = ros::Time::now();


        start.makeStartOrEnd(Eigen::Vector3d(x_1,y_1,z_1), derivative_to_optimize);
		vertices.push_back(start);

		middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION, Eigen::Vector3d(x_2,y_2,z_2));
		vertices.push_back(middle);

		middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION, Eigen::Vector3d(x_3,y_3,z_3));
		vertices.push_back(middle);

//		middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION, Eigen::Vector3d(x_4,y_4,z_4));
//		vertices.push_back(middle);

		end.makeStartOrEnd(Eigen::Vector3d(x_5,y_5,z_5), derivative_to_optimize);
		vertices.push_back(end);

		std::vector<double> segment_times;
		const double v_max = 1.0;
		const double a_max = 1.0;
		segment_times = estimateSegmentTimes(vertices, v_max, a_max);

        //N denotes the number of coefficients of the underlying polynomial
        //N has to be even.
        //If we want the trajectories to be snap-continuous, N needs to be at least 10.

        const int N = 10;
        mav_trajectory_generation::PolynomialOptimization<N> opt(dimension);
        opt.setupFromVertices(vertices, segment_times, derivative_to_optimize);
        opt.solveLinear();

        // ROS_INFO("Take %f sec to get optimal traject", (ros::Time::now() - t0).toSec());

        //Obtain the polynomial segments
        mav_trajectory_generation::Segment::Vector segments;
        opt.getSegments(&segments);

        //creating Trajectories
        
        mav_trajectory_generation::Trajectory trajectory;
        opt.getTrajectory(&trajectory);   
 
        mav_msgs::EigenTrajectoryPoint state;
		mav_msgs::EigenTrajectoryPoint::Vector states;

        //visualizing Trajectories
        visualization_msgs::MarkerArray markers;
        double distance = 1.6; // Distance by which to seperate additional markers. Set 0.0 to disable.
        std::string frame_id = "vicon";
        // From Trajectory class:
		mav_trajectory_generation::drawMavTrajectory(trajectory, distance, frame_id, &markers);

		int last_index_marker_array = markers.markers.size()-1;
		//std::cout << "trajectory ="  << markers.markers[last_index_marker_array].points[0]<< std::endl;

		//std::cout << "points ="  << markers.markers[last_index_marker_array].points[0]<< std::endl;

		int points_size = markers.markers[last_index_marker_array].points.size()-1;

		
		for (int i=0 ; i<points_size ; i++)
		{	
			point.position.x = markers.markers[last_index_marker_array].points[i].x;
			point.position.y = markers.markers[last_index_marker_array].points[i].y;
			point.position.z = markers.markers[last_index_marker_array].points[i].z;
			//std::cout <<point<< std::endl;
			points.poses.push_back(point);
		}
		//std::cout << "points ="  << markers.markers[last_index_marker_array].points[0]<< std::endl;
		points.header.frame_id = frame_id;

		traj_points_pub.publish(points);
		//point.clear();

        ros::spinOnce();

        vertices.clear();
        points.poses.clear();
        sleep_rate.sleep();
    }
    return 0;
}
