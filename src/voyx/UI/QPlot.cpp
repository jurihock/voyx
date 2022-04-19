#include <voyx/UI/QPlot.h>

#include <voyx/Source.h>

namespace args
{
  char arg0[] = "voyx";
  char* argv[] = { arg0, nullptr };
  int argc = sizeof(argv) / sizeof(char*) - 1;
}

QPlot::QPlot(const std::chrono::duration<double> delay) :
  delay(delay)
{
  application = std::make_shared<QApplication>(args::argc, args::argv);
  widget = std::make_shared<QWidget>();

  layout = std::make_shared<QGridLayout>();
  widget->setLayout(layout.get());

  addPlot(0, 0, 1);

  start();
}

QPlot::~QPlot()
{
  stop();
}

void QPlot::start()
{
  stop();

  doloop = true;

  thread = std::make_shared<std::thread>(
    [&](){ loop(); });
}

void QPlot::stop()
{
  doloop = false;

  if (thread != nullptr)
  {
    if (thread->joinable())
    {
      thread->join();
    }

    thread = nullptr;
  }
}

void QPlot::show()
{
  show(800, 600);
}

void QPlot::show(const size_t width, const size_t height)
{
  widget->setWindowTitle(args::arg0);
  widget->resize(static_cast<int>(width), static_cast<int>(height));

  widget->show();
  application->exec();
}

void QPlot::xlim(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.xauto = false;

  for (auto plot : plots)
  {
    plot->xAxis->setRange(min, max);
  }
}

void QPlot::ylim(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.yauto = false;

  for (auto plot : plots)
  {
    plot->yAxis->setRange(min, max);
  }
}

void QPlot::xrange(const double max)
{
  std::lock_guard lock(mutex);
  data.xrange = std::pair<double, double>(0, max);
}

void QPlot::xrange(const double min, const double max)
{
  std::lock_guard lock(mutex);
  data.xrange = std::pair<double, double>(min, max);
}

void QPlot::plot(const std::vector<float>& y)
{
  std::lock_guard lock(mutex);
  data.ydata = std::vector<double>(y.begin(), y.end());
}

void QPlot::plot(const std::vector<double>& y)
{
  std::lock_guard lock(mutex);
  data.ydata = y;
}

void QPlot::addPlot(const size_t row, const size_t col, const size_t graphs)
{
  auto plot = std::make_shared<QCustomPlot>();

  for (size_t i = 0; i < graphs; ++i)
  {
    plot->addGraph();

    QPen pen;

    pen.setColor(getLineColor(i));
    pen.setWidth(getLineWidth(i));

    plot->graph(i)->setPen(pen);
  }

  layout->addWidget(plot.get(), row, col);

  plots.push_back(plot);
}

QCustomPlot* QPlot::getPlot(const size_t row, const size_t col) const
{
  return dynamic_cast<QCustomPlot*>(
    layout->itemAtPosition(row, col)->widget());
}

QColor QPlot::getLineColor(const size_t index) const
{
  const auto& color = colors[index % 10];

  return QColor(color[0], color[1], color[2]);
}

int QPlot::getLineWidth(const size_t index) const
{
  return 2;
}

void QPlot::loop()
{
  const size_t row = 0;
  const size_t col = 0;
  const size_t graph = 0;

  std::array<std::chrono::milliseconds, 2> delays =
  {
    std::chrono::duration_cast<std::chrono::milliseconds>(delay),
    std::chrono::milliseconds(0)
  };

  while (doloop)
  {
    const auto delay = *std::max_element(
      delays.begin(), delays.end());

    std::this_thread::sleep_for(delay);

    bool xauto, yauto;
    std::optional<std::pair<double, double>> xrange;
    std::vector<double> ydata;
    {
      std::lock_guard lock(mutex);
      xauto = data.xauto;
      yauto = data.yauto;
      xrange = data.xrange;
      ydata = data.ydata;
    }

    std::vector<double> xdata;
    {
      xdata.resize(ydata.size());

      std::iota(xdata.begin(), xdata.end(), 0.0);

      if (xrange.has_value())
      {
        const double foo = (xrange.value().second - xrange.value().first) / xdata.size();
        const double bar = xrange.value().first;

        std::transform(xdata.begin(), xdata.end(), xdata.begin(),
          [foo, bar](double value) { return value * foo + bar; });
      }
    }

    auto plot = getPlot(row, col);
    {
      // x,y data

      const size_t size = std::min(
        xdata.size(), ydata.size());

      QVector<double> x(size), y(size);

      for (size_t i = 0; i < size; ++i)
      {
        x[i] = xdata[i];
        y[i] = ydata[i];
      }

      plot->graph(graph)->setData(x, y);

      // auto range

      if (xauto && !xdata.empty())
      {
        const double xmin = *std::min_element(xdata.begin(), xdata.end());
        const double xmax = *std::max_element(xdata.begin(), xdata.end());

        // sync again
        {
          std::lock_guard lock(mutex);

          if (data.xauto)
          {
            plot->xAxis->setRange(xmin, xmax);
          }
        }
      }

      if (yauto && !ydata.empty())
      {
        const double ymin = *std::min_element(ydata.begin(), ydata.end());
        const double ymax = *std::max_element(ydata.begin(), ydata.end());

        // sync again
        {
          std::lock_guard lock(mutex);

          if (data.yauto)
          {
            plot->yAxis->setRange(ymin, ymax);
          }
        }
      }

      // replot

      plot->replot(QCustomPlot::rpQueuedReplot);

      delays.back() = std::chrono::milliseconds(
        static_cast<int>(plot->replotTime()));
    }
  }
}
