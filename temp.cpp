CREATE OR REPLACE FUNCTION upsert_pm(
    in_nm TEXT,
    in_desc TEXT,
    in_fltr JSONB,
    in_dev TEXT,
    in_rnd TEXT,
    in_io BOOLEAN
) RETURNS INTEGER AS
$$
DECLARE
    existing_id INTEGER;
    existing_fltr JSONB;
    new_fltr JSONB;
    report_type INT;
    existing_var_part TEXT;
    new_var_part TEXT;
    base_var_part TEXT;
    last_number INT;
    parts TEXT[];
BEGIN
    -- Try to get existing row by name
    SELECT id, fltr INTO existing_id, existing_fltr
    FROM public.pm
    WHERE nm = in_nm;

    IF existing_id IS NOT NULL THEN
        -- Extract report_type and var_part
        report_type := (in_fltr ->> 'report_type')::INT;

        IF (existing_fltr ->> 'report_type')::INT = report_type THEN
            existing_var_part := existing_fltr ->> 'var_part';

            -- Split existing var_part by '#' to get parts
            parts := string_to_array(existing_var_part, '#');

            -- Get last numeric part and increment
            last_number := parts[array_length(parts, 1)]::INT + 1;

            -- Create new var_part by appending #<next_number>
            new_var_part := existing_var_part || '#' || last_number::TEXT;

            -- Set it in the incoming fltr JSON
            new_fltr := jsonb_set(in_fltr, '{var_part}', to_jsonb(new_var_part::TEXT), false);
        ELSE
            -- If report_type doesn't match, keep incoming fltr as is
            new_fltr := in_fltr;
        END IF;

        -- Update existing row
        UPDATE public.pm
        SET fltr = new_fltr,
            "desc" = in_desc,
            type = in_rnd,
            io = in_io
        WHERE id = existing_id
        RETURNING id INTO existing_id;

        RETURN existing_id;
    ELSE
        -- Insert new row
        INSERT INTO public.pm(nm, "desc", fltr, sv_dev, glb, type, io, en)
        VALUES (
            in_nm,
            in_desc,
            in_fltr,
            (SELECT id FROM public.ldev WHERE nm = in_dev),
            FALSE,
            in_rnd,
            in_io,
            TRUE
        )
        RETURNING id INTO existing_id;

        RETURN existing_id;
    END IF;
END;
$$ LANGUAGE plpgsql;




QSqlQuery query;
query.prepare(R"(
    SELECT upsert_pm(
        :nm,
        :desc,
        :fltr,
        :dev,
        :rnd,
        :io
    ) AS pmId
)");

query.bindValue(":nm", name);
query.bindValue(":desc", description);
query.bindValue(":fltr", QJsonDocument(fltrJsonObject).toJson(QJsonDocument::Compact));  // fltr is a JSON object
query.bindValue(":dev", deviceName);
query.bindValue(":rnd", typeString);
query.bindValue(":io", ioBool);

if (!query.exec()) {
    qDebug() << "Error executing query:" << query.lastError().text();
} else {
    if (query.next()) {
        int pmId = query.value("pmId").toInt();
        qDebug() << "Returned pmId:" << pmId;
    }
}





SELECT upsert_pm(
    'SignalA',
    'Initial Insert',
    '{
        "type": "name1",
        "numbit": 12,
        "offset": 6,
        "numword": 9,
        "countbit": 2,
        "var_part": "70",
        "report_type": 7,
        "internalname": "somename1"
    }'::jsonb,
    'DeviceA',
    'RandomType',
    true
);