extends Node2D

@onready var sprite: Sprite2D = $Sprite2D
@onready var scale_input: SpinBox = $ControlsPanel/ScaleInput
@onready var apply_button: Button = $ControlsPanel/ApplyButton
@onready var label: Label = $ControlsPanel/Label

var original_image: Image
var processor: DitherProcessor
var current_size: Vector2i = Vector2i.ZERO
var original_sprite_size: Vector2i = Vector2i.ZERO

func _ready():
    processor = DitherProcessor.new()

    original_image = sprite.texture.get_image()
    original_sprite_size = original_image.get_size()

    apply_button.pressed.connect(_on_apply_pressed)

    _update_texture(original_image.get_size())

func _on_apply_pressed() -> void:
    _update_texture(original_sprite_size * scale_input.value)

func _update_texture(target_size: Vector2i) -> void:
    target_size.x = max(4, target_size.x)
    target_size.y = max(4, target_size.y)
    if target_size == current_size:
        return

    current_size = target_size

    var resized = original_image.duplicate()
    resized.resize(target_size.x, target_size.y, Image.INTERPOLATE_NEAREST)

    var time_start = Time.get_ticks_msec()
    var result_image = processor.process(resized)
    var time_end = Time.get_ticks_msec()
    label.text = "%d ms" % (time_end - time_start)

    sprite.texture = ImageTexture.create_from_image(result_image)
