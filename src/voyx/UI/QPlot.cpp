#include <voyx/UI/QPlot.h>

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

void QPlot::plot(const std::vector<float>& y)
{
  std::unique_lock lock(mutex);
  data = y;
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
  std::vector<std::chrono::milliseconds> delays =
  {
    std::chrono::milliseconds(0),
    std::chrono::duration_cast<std::chrono::milliseconds>(delay),
  };

  std::vector<double> xminmax(2);
  std::vector<double> yminmax(2);

  while (doloop)
  {
    const auto delay = *std::max_element(
      delays.begin(), delays.end());

    std::this_thread::sleep_for(delay);

    std::vector<float> ydata;
    {
      std::unique_lock lock(mutex);
      ydata = data;
    }

    std::vector<float> xdata;
    {
      xdata.resize(ydata.size());

      for (size_t i = 0; i < xdata.size(); ++i)
      {
        xdata[i] = i;
      }
    }

    const size_t row = 0;
    const size_t col = 0;
    const size_t graph = 0;
    {
      auto plot = getPlot(row, col);

      // data

      const size_t n = std::min(
        xdata.size(), ydata.size());

      QVector<double> x(n), y(n);

      for (size_t i = 0; i < n; ++i)
      {
        x[i] = static_cast<double>(xdata[i]);
        y[i] = static_cast<double>(ydata[i]);
      }

      plot->graph(graph)->setData(x, y);

      // range

      if (!xdata.empty())
      {
        const double xmin = static_cast<double>(*std::min_element(xdata.begin(), xdata.end()));
        const double xmax = static_cast<double>(*std::max_element(xdata.begin(), xdata.end()));

        xminmax[0] = std::min(xminmax[0], xmin);
        xminmax[1] = std::max(xminmax[1], xmax);
      }

      if (!ydata.empty())
      {
        const double ymin = static_cast<double>(*std::min_element(ydata.begin(), ydata.end()));
        const double ymax = static_cast<double>(*std::max_element(ydata.begin(), ydata.end()));

        yminmax[0] = std::min(yminmax[0], ymin);
        yminmax[1] = std::max(yminmax[1], ymax);
      }

      yminmax[0] = std::min(yminmax[0], -1.0);
      yminmax[1] = std::max(yminmax[1], +1.0);

      plot->xAxis->setRange(xminmax[0], xminmax[1]);
      plot->yAxis->setRange(yminmax[0], yminmax[1]);

      // replot

      plot->replot(QCustomPlot::rpQueuedReplot);

      delays[0] = std::chrono::milliseconds(
        static_cast<int>(plot->replotTime()));
    }
  }
}
