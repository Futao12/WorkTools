//#include <iostream>
//#include <vector>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <chrono>
//
//template <typename T>
//class CircularQueue final{
//public:
//    CircularQueue(int capacity) : _capacity(capacity), _head(0), _tail(0), _size(0), _queue(capacity) {}
//    void push(const T& data) {
//        std::unique_lock<std::mutex> lock(_mutex);
//        // 缓冲区已满，等待
//        while (_size == _capacity) {
//            _cond_full.wait(lock);
//        }
//        _queue[_tail] = data;
//        _tail = (_tail + 1) % _capacity;
//        ++_size;
//        _cond_empty.notify_one();  // 唤醒等待读取数据的消费者
//    }
//    std::pair<T, T> pop() {
//        std::unique_lock<std::mutex> lock(_mutex);
//        // 缓冲区为空，等待
//        while (_size == 0) {
//            _cond_empty.wait(lock);
//        }
//
//        T dataHead = _queue[_head];
//        _head = (_head + 1) % _capacity;
//        --_size;
//        T dataTail = _queue[(_tail - 1 + _capacity) % _capacity];
//        _cond_full.notify_one();  // 唤醒等待写入数
//        return std::make_pair(dataHead, dataTail);
//    }
//
//    int getCapacity() {
//        return _capacity;
//    }
//
//private:
//    const int _capacity;
//    int _head;
//    int _tail;
//    int _size;
//    std::vector<T> _queue;
//    std::mutex _mutex;
//    std::condition_variable _cond_empty;
//    std::condition_variable _cond_full;
//};
//
////template <typename T>
////class CircularQueue {
////public:
////    CircularQueue(int capacity) : _capacity(capacity), _head(0), _tail(0), _size(0), _queue(capacity) {}
////
////    void push(const T& data) {
////        std::unique_lock<std::mutex> lock_push(_mutex_push);
////        while (_size == _capacity) {
////            _cond_push.wait(lock_push);
////        }
////
////        _queue[_tail] = data;
////        _tail = (_tail + 1) % _capacity;
////        ++_size;
////
////        _cond_pop.notify_one();  // 唤醒等待读取数据的消费者
////    }
////
////    std::pair<T, T> popAndReadTail() {
////        std::unique_lock<std::mutex> lock_pop(_mutex_pop);
////        while ( _size == 0) {
////            _cond_pop.wait(lock_pop);
////        }
////        T dataHead = _queue[_head];
////        _head = (_head + 1) % _capacity;
////        --_size;
////        T dataTail = _queue[_tail];
////        _cond_push.notify_one();  // 唤醒等待写入数据的生产者
////        return std::make_pair(dataHead, dataTail);
////    }
////
////    int getCapacity() {
////        return _capacity;
////    }
////
////
////private:
////    const int _capacity;
////    int _head;
////    int _tail;
////    int _size;
////    struct syncData{
////        T headData;
////        T tailData;
////    };
////    std::vector<T> _queue;
////
////    std::mutex _mutex_push;
////    std::mutex _mutex_pop;
////
////    std::condition_variable _cond_push;
////    std::condition_variable _cond_pop;
////};
//
//typedef 
//struct Signal final
//{
//	long long sigVal;
//	int  index; // one index one micron
//    bool ACCFlag {false};
//}SIG_t;
//
//// 生成高斯分布值的函数
//double gaussian(double x, double mean, double stddev) {
//    return exp(-pow(x - mean, 2) / (2 * pow(stddev, 2)));
//}
//
//// 生成高斯信号的函数
//std::vector<SIG_t> generateGaussianSignal(int numPoints, long long peakValue, double mean, double stddev, bool ACCFlag) {
//    std::vector<SIG_t> gaussianSignal;
//
//    // 生成高斯信号并填充结构体数组
//    for (int i = 0; i < numPoints; ++i) {
//        SIG_t pointSignal;
//        pointSignal.sigVal = static_cast<long long>(std::round(gaussian(i, mean, stddev) * peakValue));  // 乘以1000.0用于将小数转换为整数
//        pointSignal.index = i;
//        if (pointSignal.sigVal > 0) {
//            pointSignal.ACCFlag = ACCFlag;
//        }
//
//        gaussianSignal.push_back(pointSignal);
//    }
//
//    return gaussianSignal;
//}
//
//std::vector<SIG_t> GenerateAllGaussian() {
//    //const int numPoints = 80;
//    //const double mean = 40;  // 均值
//    //const double stddev = 2.5;  // 标准差
//
//    // 调用生成高斯信号的函数
//    std::vector<SIG_t> GaussianA = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    std::vector<SIG_t> GaussianB = generateGaussianSignal(80, static_cast<long long>(2000), 40, 2.5, true);
//    for (auto& sig : GaussianB) { sig.index += 130; }
//    std::vector<SIG_t> GaussianC = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    for (auto& sig : GaussianC) { sig.index += 300; }
//
//    std::vector<SIG_t> GaussianD = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    for (auto& sig : GaussianD) { sig.index += 600; }
//    std::vector<SIG_t> GaussianE = generateGaussianSignal(80, static_cast<long long>(2000), 40, 2.5, true);
//    for (auto& sig : GaussianE) { sig.index += 730; }
//    std::vector<SIG_t> GaussianF = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    for (auto& sig : GaussianF) { sig.index += 900; }
//
//    std::vector<SIG_t> GaussianG = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    for (auto& sig : GaussianG) { sig.index += 1455; }
//    std::vector<SIG_t> GaussianH = generateGaussianSignal(80, static_cast<long long>(2000), 40, 2.5, true);
//    for (auto& sig : GaussianH) { sig.index += 1585; }
//    std::vector<SIG_t> GaussianI = generateGaussianSignal(40, static_cast<long long>(1000), 20, 2.5, false);
//    for (auto& sig : GaussianI) { sig.index += 1755; }
//
//    std::vector<SIG_t> Gaussian;
//    Gaussian.insert(Gaussian.end(), GaussianA.begin(), GaussianA.end());
//    Gaussian.insert(Gaussian.end(), GaussianB.begin(), GaussianB.end());
//    Gaussian.insert(Gaussian.end(), GaussianC.begin(), GaussianC.end());
//
//    Gaussian.insert(Gaussian.end(), GaussianD.begin(), GaussianD.end());
//    Gaussian.insert(Gaussian.end(), GaussianE.begin(), GaussianE.end());
//    Gaussian.insert(Gaussian.end(), GaussianF.begin(), GaussianF.end());
//
//    Gaussian.insert(Gaussian.end(), GaussianG.begin(), GaussianG.end());
//    Gaussian.insert(Gaussian.end(), GaussianH.begin(), GaussianH.end());
//    Gaussian.insert(Gaussian.end(), GaussianI.begin(), GaussianI.end());
//
//    return Gaussian;
//}
//
//
//int Index = 0;
//CircularQueue<SIG_t> q(151);  // 缓冲区容量为10
//SIG_t tailDataTmp = {-1, -1, false};
//
//void producer(size_t vecSize, const std::vector<SIG_t>& sourceData) {
//    while (Index < vecSize) {
//        if (Index == 1820) {
//            break;
//        }
//        Index++;
//        tailDataTmp = sourceData[Index];
//        q.push(tailDataTmp);
//    }
//}
//
//std::vector<SIG_t> result;
//const int peakRatio = 2;
//
//void consumer() {
//    while (true) {
//       /* SIG_t signalHead = q.pop();
//        if (signalHead.index == 1820) {
//            break;
//        }*/
//        
//        SIG_t signalTail = tailDataTmp;
//        auto syncData = q.pop();
//        SIG_t signalHead = syncData.first;
//        if (signalHead.index == 1820) {
//            break;
//        }
//
//        auto lowThreShold1  = peakRatio * signalTail.sigVal * 0.6;
//        auto highThreShold1 = peakRatio * signalTail.sigVal * 1.20;
//        bool cond1 = (signalHead.sigVal <= highThreShold1) && (signalHead.sigVal >= lowThreShold1);
//        bool cond2 = (signalHead.sigVal != 0);
//        if (cond1 && cond2) {
//            result.emplace_back(signalHead);
//        }
//
//        /*if (signalHead.ACCFlag) {
//            auto tempVal = signalHead.sigVal * 1.5;
//            std::cout << "Processed: Index: " << signalHead.index << ", Signal Value: " << tempVal << ", ACCFlag: " << (signalHead.ACCFlag ? "true" : "false") << std::endl;
//        }
//        else {
//            std::cout << "Unmodified: Index: " << signalHead.index << ", Signal Value: " << signalHead.sigVal << ", ACCFlag: " << (signalHead.ACCFlag ? "true" : "false") << std::endl;
//        }*/
//    }
//}
//
//
//int main() {
//    // 输出生成的高斯信号
//    std::vector<SIG_t> Gaussian =  GenerateAllGaussian();
//    size_t GSize = Gaussian.size();
//    std::vector<SIG_t> GaussianFull;
//
//    for (size_t i = 0; i < (1794 + 33); ++i) {
//        GaussianFull.emplace_back(SIG_t{ 0, static_cast<int>(i), false });
//    }
//   
//    for (const auto& pointSignal : Gaussian) {
//        int pointSignalindex = pointSignal.index;
//        for (auto& signal : GaussianFull) {
//            if (signal.index == pointSignalindex) {
//                signal.ACCFlag = pointSignal.ACCFlag;
//                signal.index   = pointSignal.index;
//                signal.sigVal  = pointSignal.sigVal;
//            }
//        }
//    }
//
//    /*for (const auto& signal : GaussianFull) {
//        std::cout << "Index: " << signal.index << ", Signal Value: " << signal.sigVal << ", ACCFlag: " << (signal.ACCFlag ? "true" : "false") << std::endl;
//    }*/
//
//    const double ratio = 1.5;  // 你的比率值
//
//    // 假设有一个包含 Signal_t 的 vector 作为数据源
//    std::vector<SIG_t> sourceData = GaussianFull;
//    int vecSize = sourceData.size();
//    
//    auto start_time = std::chrono::high_resolution_clock::now();
//
//    std::thread t1(producer, std::ref(vecSize), std::ref(sourceData));
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    int capacity = q.getCapacity();
//    std::cout << "Circle queue's capacity is: " << capacity << std::endl;
//
//    std::thread t2(consumer);
//    //t1.join();
//    try {
//        t1.join();
//        std::cout << "Thread1 joined successfully." << std::endl;
//    }
//    catch (const std::system_error& e) {
//        std::cerr << "Failed to join thread1: " << e.what() << std::endl;
//    }
//    try {
//        t2.join();
//        std::cout << "Thread2 joined successfully." << std::endl;
//    }
//    catch (const std::system_error& e) {
//        std::cerr << "Failed to join thread2: " << e.what() << std::endl;
//    }
//
//
//    auto end_time = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
//    std::cout << "Time taken: " << duration.count() << " milliseconds." << std::endl;
//
//    for (const auto& sigRes : result) {
//        std::cout << "Peak index: " << sigRes.index << ", Signal Value: " << sigRes.sigVal << ", ACCFlag: " << (sigRes.ACCFlag ? "true" : "false") << std::endl;
//    }
//
//    int resCapacity = result.size();
//    std::cout << "result vector's capacity is: " << resCapacity << std::endl;
//
//    return 0;
//}