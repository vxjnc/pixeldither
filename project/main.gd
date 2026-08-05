extends Node2D

@onready var sprite: Sprite2D = $Sprite2D
@onready var scale_input: SpinBox = $ControlsPanel/ScaleInput
@onready var apply_button: Button = $ControlsPanel/ApplyButton
@onready var label: Label = $ControlsPanel/Label
@onready var factor_input: SpinBox = $ControlsPanel/FactorInput
@onready var method_input: OptionButton = $ControlsPanel/MethodInput

var original_image: Image
var processor: DitherProcessor
var current_size: Vector2i = Vector2i.ZERO
var current_factor: float = 0.0
var current_method: DitherProcessor.DitherMethod = DitherProcessor.DitherMethod.DITHER_ERROR_DIFFUSION
var original_sprite_size: Vector2i = Vector2i.ZERO

func _ready():
    processor = DitherProcessor.new()

    original_image = sprite.texture.get_image()
    original_sprite_size = original_image.get_size()

    apply_button.pressed.connect(_on_apply_pressed)

    method_input.add_item("Error Diffusion", DitherProcessor.DitherMethod.DITHER_ERROR_DIFFUSION)
    method_input.add_item("Old (Bayer 8x8)", DitherProcessor.DitherMethod.DITHER_OLD_DITHERING)

    _update_texture(factor_input.value, original_image.get_size(), current_method)

func _on_apply_pressed() -> void:
    var selected_method := method_input.get_selected_id() as DitherProcessor.DitherMethod
    _update_texture(factor_input.value, original_sprite_size * scale_input.value, selected_method)

func _update_texture(factor: float, target_size: Vector2i, method: DitherProcessor.DitherMethod) -> void:
    target_size.x = max(4, target_size.x)
    target_size.y = max(4, target_size.y)
    if target_size == current_size and factor == current_factor and method == current_method:
        return

    current_size = target_size
    current_factor = factor
    current_method = method

    var resized = original_image.duplicate()
    resized.resize(target_size.x, target_size.y, Image.INTERPOLATE_NEAREST)
    resized.convert(Image.FORMAT_LA8)

    var time_start = Time.get_ticks_msec()
    var result_image = processor.process(factor, resized, method)
    var time_end = Time.get_ticks_msec()
    label.text = "%d ms" % (time_end - time_start)

    sprite.texture = ImageTexture.create_from_image(result_image)
