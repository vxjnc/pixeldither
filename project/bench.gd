@tool
extends EditorScript

func _run():
    var processor = DitherProcessor.new()
    var test_image = Image.create(2048, 2048, false, Image.FORMAT_LA8)
    test_image.fill(Color(0.5, 0.5, 0.5, 1.0))

    var sizes = [64, 128, 256, 512, 1024, 2048]
    var iterations = 10
    var warmup_iterations = 3

    for size in sizes:
        var resized = test_image.duplicate()
        resized.resize(size, size, Image.INTERPOLATE_NEAREST)

        for i in range(warmup_iterations):
            processor.process(resized)

        var times: Array[float] = []
        for i in range(iterations):
            var start = Time.get_ticks_usec()
            processor.process(resized)
            var elapsed = (Time.get_ticks_usec() - start) / 1000.0
            times.append(elapsed)

        var total = 0.0
        var min_t = times[0]
        var max_t = times[0]
        for t in times:
            total += t
            min_t = min(min_t, t)
            max_t = max(max_t, t)

        print("Size %dx%d: avg=%.3fms min=%.3fms max=%.3fms" % [size, size, total / iterations, min_t, max_t])
