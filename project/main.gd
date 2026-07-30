extends Node2D

@onready var sprite: Sprite2D = $Sprite2D

var original_image: Image
var processor: DitherProcessor
var current_target_size: Vector2i = Vector2i.ZERO

var dragging: bool = false
var drag_handle_size: float = 16.0

func _ready():
    processor = DitherProcessor.new()
    original_image = sprite.texture.get_image()
    sprite.centered = false

    _update_texture(original_image.get_size())

func _update_texture(target_size: Vector2i) -> void:
    target_size.x = max(4, target_size.x)
    target_size.y = max(4, target_size.y)
    if target_size == current_target_size:
        return
    current_target_size = target_size
    var result_image = processor.process(original_image, target_size.x, target_size.y)
    sprite.texture = ImageTexture.create_from_image(result_image)

func _get_bottom_right() -> Vector2:
    return sprite.position + Vector2(current_target_size)

func _input(event: InputEvent) -> void:
    if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
        if event.pressed:
            var handle_pos = _get_bottom_right()
            if event.position.distance_to(handle_pos) < drag_handle_size:
                dragging = true
        else:
            dragging = false

    elif event is InputEventMouseMotion and dragging:
        var new_size = event.position - sprite.position
        _update_texture(Vector2i(new_size))
