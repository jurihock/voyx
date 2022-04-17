#pragma once

#include <voyx/UI/Plot.h>

#include <qcustomplot/qcustomplot.h>

#include <QApplication>
#include <QColor>
#include <QGridLayout>
#include <QPen>
#include <QWidget>

#include <algorithm>
#include <array>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

class QPlot : public Plot
{

public:

  QPlot(const std::chrono::duration<double> delay = std::chrono::duration<double>::zero());
  ~QPlot();

  void start();
  void stop();

  void show() override;
  void show(const size_t width, const size_t height);

  void lim(const double min, const double max) override;

  void plot(const std::vector<float>& y) override;

protected:

  void addPlot(const size_t row, const size_t col, const size_t graphs);
  QCustomPlot* getPlot(const size_t row, const size_t col) const;
  QColor getLineColor(const size_t index) const;
  int getLineWidth(const size_t index) const;

private:

  const std::chrono::duration<double> delay;

  std::shared_ptr<QApplication> application;
  std::shared_ptr<QWidget> widget;
  std::shared_ptr<QGridLayout> layout;
  std::vector<std::shared_ptr<QCustomPlot>> plots;

  struct
  {
    std::vector<float> ydata;
  }
  data;

  std::shared_ptr<std::thread> thread;
  std::mutex mutex;
  bool doloop = false;

  void loop();

  /*
    #!/usr/bin/env python3
    import numpy
    import seaborn
    colors = numpy.array(seaborn.color_palette())
    colors = (colors * 0xFF).clip(0, 0xFF).astype(numpy.uint8)
    colors = colors.tolist()
    print(colors)
  */

  const int colors[10][3] =
  {
    {  31, 119, 180 },
    { 255, 127,  14 },
    {  44, 160,  44 },
    { 214,  39,  40 },
    { 148, 103, 189 },
    { 140,  86,  75 },
    { 227, 119, 194 },
    { 127, 127, 127 },
    { 188, 189,  34 },
    {  23, 190, 207 },
  };

};
