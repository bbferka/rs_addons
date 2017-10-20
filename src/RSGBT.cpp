// Developed by: Rakib

#include<iostream>
#include <vector>
#include <fstream>
#include <string>
#include "opencv2/opencv.hpp"
#include <map>
#include <yaml-cpp/yaml.h>
#include<ros/package.h>
#include<boost/filesystem.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <rs_addons/RSClassifier.h>
#include <rs_addons/RSGBT.h>

using namespace cv;

//....................................Gradient Boost Trees........................................
RSGBT::RSGBT()
{
}

void RSGBT:: trainModel(std::string train_matrix_name, std::string train_label_name, std::string trained_file_name)
{
  cv::Mat train_matrix;
  cv::Mat train_label;
  readDescriptorAndLabel(train_matrix_name, train_label_name, train_matrix, train_label);
  std::cout << "size of train matrix:" << train_matrix.size() << std::endl;
  std::cout << "size of train label:" << train_label.size() << std::endl;
  std::string pathToSaveModel= saveTrained(trained_file_name);

  if(!pathToSaveModel.empty())
  {
    cv::Mat var_type = cv::Mat(train_matrix.cols + 1, 1, CV_8U);
    var_type.setTo(Scalar(CV_VAR_NUMERICAL));
    var_type.at<uchar>(train_matrix.cols, 0) = CV_VAR_CATEGORICAL;

    //Set parameters......................................
    CvGBTreesParams params
        (  CvGBTrees::DEVIANCE_LOSS, // loss_function_type
           200, // weak_count
           0.8f, // shrinkage
           1.0f, // subsample_portion
           20, // max_depth
           false // use_surrogates )
           );

    //params.max_categories =15;

    CvGBTrees *gbtree = new CvGBTrees;

    //train the random forest.....................................
    gbtree->train(train_matrix, CV_ROW_SAMPLE, train_label, cv::Mat(), cv::Mat(), var_type, cv::Mat(), params);

    //To save the trained data.............................
    gbtree->save((pathToSaveModel).c_str());
  }
}

void RSGBT:: classify(std::string trained_file_name_saved, std::string test_matrix_name, std::string test_label_name, std::string obj_classInDouble)
{
  cv::Mat test_matrix;
  cv::Mat test_label;
  readDescriptorAndLabel(test_matrix_name, test_label_name, test_matrix, test_label);
  std::cout << "size of test matrix :" << test_matrix.size() << std::endl;
  std::cout << "size of test label" << test_label.size() << std::endl;

  CvGBTrees *brtree = new CvGBTrees;

  //To load the trained model
  brtree->load((loadTrained(trained_file_name_saved)).c_str());

  //Convert test label matrix into a vector.......................
  std::vector<double> con_test_label;
  test_label.col(0).copyTo(con_test_label);

  //Container to hold the integer value of labels............................
  std::vector<int> actual_label;
  std::vector<int> predicted_label;

  for(int i = 0; i < test_label.rows; i++)
  {
    double res = brtree->predict(test_matrix.row(i), cv::Mat());
    int prediction = res;
    predicted_label.push_back(prediction);
    double lab = con_test_label[i];
    int actual_convert = lab;
    actual_label.push_back(actual_convert);
  }
  std::cout << "Gradient Boost Tree Result :" << std::endl;
  evaluation(actual_label, predicted_label, obj_classInDouble);

}

void RSGBT::classifyOnLiveData(std::string trained_file_name_saved, cv::Mat test_mat, double &det)
{
  //To load the test data and it's label.............................
  std::cout << "size of test matrix :" << test_mat.size() << std::endl;

  CvGBTrees *urtree = new CvGBTrees;

  //To load the trained data................................
  urtree->load((loadTrained(trained_file_name_saved)).c_str());
  double res = urtree->predict(test_mat, cv::Mat());
  std::cout << "prediction class is :" << res << std::endl;
  det = res;
}

void RSGBT::RsAnnotation(uima::CAS &tcas, std::string class_name, std::string feature_name, std::string database_name, rs::Cluster &cluster, std::string set_mode)
{
  rs::Classification classResult = rs::create<rs::Classification>(tcas);
  classResult.classname.set(class_name);
  classResult.classifier("Gradient Boost Tree");
  classResult.featurename(feature_name);
  classResult.model(database_name);

  if(set_mode == "CL")
  {
    //To annotate the clusters..................
    cluster.annotations.append(classResult);
  }
  else if(set_mode == "GT")
  {
    rs::GroundTruth setGT = rs::create<rs::GroundTruth>(tcas);
    setGT.classificationGT.set(classResult);
    cluster.annotations.append(setGT);
  }
  else
  {
    outError("You should set the parameter (set_mode) as CL or GT"<<std::endl);
  }
}

RSGBT::~ RSGBT()
{
}
