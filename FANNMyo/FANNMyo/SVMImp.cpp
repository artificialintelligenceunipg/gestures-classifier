//
//  SVMImp.cpp
//  FANNMyo
//
//  Created by Gabriele Di Bari on 29/09/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//

#include "SVMImp.h"
#include "SVMSerialize.h"

namespace myosvm
{

void create_model(const std::string& path)
{
    svm_problem* dataset = svm_deserialize(path);
    svm_parameter param = {0};
    
#if 0
    param.svm_type    = NU_SVC;//C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 200.0f;
    param.coef0       = 0.2f;
    param.degree      = 10;
    param.eps         = 0.1f;
    param.gamma       = 0.121f;
    param.nu          = 0.1f;
    param.probability = true;
    param.shrinking   = true;
#elif 1
    
    param.svm_type    = NU_SVC;//C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 100.0f;
    param.coef0       = 0.1f;
    param.degree      = 13;
    param.eps         = 0.1f;
    param.gamma       = 0.12f;
    param.nu          = 0.1001f;
    param.probability = true;
    param.shrinking   = false;
#elif 0
    param.svm_type    = NU_SVC;//C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 100.0f;
    param.coef0       = 0.1f;
    param.degree      = 10;
    param.eps         = 0.1f;
    param.gamma       = 0.05f;
    param.nu          = 0.001f;
    param.probability = true;
    param.shrinking   = true;
#elif 0
    param.svm_type    = NU_SVC;//C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 10.0f;
    param.coef0       = 0.01f;
    param.degree      = 10;
    param.eps         = 0.2f;
    param.gamma       = 0.2f;
    param.nu          = 0.0005f;
    param.probability = true;
    param.shrinking   = true;
#elif 1
    param.svm_type    = NU_SVC;//C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 1000.0f;
    param.coef0       = 0.01f;
    param.degree      = 10;
    param.eps         = 0.1f;
    param.gamma       = 0.2f;
    param.nu          = 0.001f;
    param.probability = true;
    param.shrinking   = true;
#else
    param.svm_type    = C_SVC;
    param.kernel_type = RBF;
    param.cache_size  = 100.0f;
    param.coef0       = 0.1f;
    param.degree      = 20;
    param.eps         = 0.06f;
    param.gamma       = 0.1f;
    param.nu          = 0.1f;
    param.probability = true;
    param.shrinking   = true;
#endif
    svm_do_cross_validation(param,*dataset,10);
    svm_model* model = svm_train(dataset,&param);
    if(svm_save_model((path+".svm.model").c_str(),model))
    {
        fprintf(stderr, "can't save model to file %s\n", (path+".svm.model").c_str());
    }
    svm_free_and_destroy_model(&model);
    svm_free(dataset);
}
void* myo_classification(const std::string& path,MyoThread& myo,DataFlags&  flags,ClassesNames& cnames)
{
        svm_model* model=svm_load_model((path+".svm.model").c_str());
        //add function
        myo.start([&,model](const MyoThread::Inputs& inputs,
                            const DataFlags& falgs,
                            std::mutex& mutex)
                  {
                      if(!inputs.size()) return;
                      //loop constants
                      size_t count = 0;
                      size_t rowSize = falgs.lineSize<8>() / flags.mReps;
                      size_t allSize = rowSize*inputs.size();
                      std::unique_ptr<svm_node> ubuffer(new svm_node[allSize+1]);
                      //ptr to buffer
                      svm_node* buffer = &(*ubuffer);
                      //set last id
                      buffer[allSize].index = -1;
                      //get max time
                      double maxTime = inputs[inputs.size()-1].getTime();
                      //put values
                      for(const auto& row : inputs)
                      {
                          //index
                          size_t i = 0;
                          size_t irow = rowSize*count;
                          //
                          if(falgs.mTime)
                          {
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = row.getTime() / maxTime;
                          }
                          if(falgs.mGyroscope)
                          {
                              auto gyr = row.getGyroscope().normalized();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = gyr.x();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = gyr.y();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = gyr.z();
                          }
                          if(falgs.mAccelerometer)
                          {
                              auto acc = row.getAccelerometer().normalized();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = acc.x();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = acc.y();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = acc.z();
                              
                          }
                          if(falgs.mQuaternion)
                          {
                              auto quad = row.getQuaternion().normalized();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = quad.x();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = quad.y();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = quad.z();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = quad.w();
                          }
                          if(falgs.mEuler)
                          {
                              auto euler = row.getEulerAngles();
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = (double)euler.roll() / M_PI_2;
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = (double)euler.pitch() / M_PI_2;
                              buffer[i   + irow].index = (int)i+1 + (int)irow;
                              buffer[i++ + irow].value = (double)euler.yaw() / M_PI_2;
                          }
                          if(falgs.mEmg)
                          {
                              //TODO: normalizzare gli emg
                              for(auto emg:row.getEmg())
                              {
                                  buffer[i   + irow].index = (int)i+1 + (int)irow;
                                  buffer[i++ + irow].value = ((double)emg) / 128.0;
                              }
                          }
                          //next
                          ++count;
                      }
#if 0
                      
                      for(size_t x=0;x!=inputs.size();++x)
                      {
                          for(size_t a=0;a!=rowSize;++a)
                          {
                              size_t id = rowSize*x+a;
                              std::cout << '(' << buffer[id].index  ;
                              std::cout << ',' << buffer[id].value << ")";
                          }
                          std::cout<<"\n";
                      }
                      std::cout << "-----------------------------------------------\n";
#endif
                      //run
                      static size_t countOuput = 0;
                      std::unique_ptr<double[]> classProb(new double[model->nr_class]);
                      double calcOut = svm_predict_probability(model, buffer,&classProb[0]);
                      //print
                      std::cout << "-----------------------------------------------\n";
                      std::cout << "arm in in status: "
                                << calcOut << '(' << ++countOuput << "), name: "
                                << cnames.getClassName(calcOut) << "\n";
                      std::cout << "( ";
                      for(int c=0;c!=model->nr_class;++c)
                          std::cout << classProb[c] << " ";
                      std::cout << ")\n";
                      std::cout << "-----------------------------------------------\n";
                      std::cout.flush();

                  },flags);
    return model;
}
    
void  myo_classification_free(void* model)
{
    svm_free_model_content((svm_model*)model);
}

}